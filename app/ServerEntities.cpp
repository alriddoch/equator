// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerEntities.h"
#include "GlView.h"
#include "Server.h"
#include "Model.h"
#include "MainWindow.h"
#include "Palette.h"
#include "HeightData.h"

#include "debug.h"

#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>

#include <Atlas/Codecs/XML.h>

#include <GL/glu.h>

#include <gtk--/fileselection.h>

#include <fstream>

static const bool debug_flag = false;

using Atlas::Message::Object;

class DummyDecoder : public Atlas::Message::DecoderBase {
  private:
    virtual void ObjectArrived(const Atlas::Message::Object&) { }
  public:
    DummyDecoder() { }
};

class FileDecoder : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    Atlas::Codecs::XML m_codec;
    Object::MapType m_entities;
    int m_count;
    std::string m_topLevelId;

    virtual void ObjectArrived(const Object & obj) {
        const Object::MapType & omap = obj.AsMap();
        Object::MapType::const_iterator I;
        if (((I = omap.find("id")) == omap.end()) ||
            (!I->second.IsString()) || (I->second.AsString().empty())) {
            std::cerr << "WARNING: Object in file has no id. Not stored."
                      << std::endl << std::flush;
            return;
        }
        const std::string & id = I->second.AsString();
        if (((I = omap.find("loc")) == omap.end()) ||
            (!I->second.IsString()) || (I->second.AsString().empty())) {
            // Entity with loc attribute must be the root entity
            if (!m_topLevelId.empty()) {
                std::cerr << "ERROR: File contained TLE with id = "
                          << m_topLevelId << " and also contains another id = "
                          << id << std::endl << std::flush;
            }
            m_topLevelId = id;
        }
        m_entities["id"] = obj;
        m_count++;
    }
  public:
    FileDecoder(const std::string & filename) :
                m_file(filename.c_str(), std::ios::in),
                m_codec(m_file, this), m_count(0)
    {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.Poll();
        }
    }

    void report() {
        std::cout << m_count << " objects read from file and stored."
                  << std::endl << std::flush;
        if (m_topLevelId.empty()) {
            std::cerr << "WARNING: No TLE found" << std::endl << std::flush;
        }
    }
};

inline Atlas::Message::Object WFMath::Point<3>::toAtlas() const
{
    Atlas::Message::Object::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Object(ret);
}

void ServerEntities::draw3DCube(const WFMath::Point<3> & coords,
                                const WFMath::AxisBox<3> & bbox, bool open)
{
    bool shaded = (m_renderMode == GlView::SHADED);
    glPushMatrix();

    glTranslatef(coords.x(), coords.y(), coords.z());

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    // Bottom face
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    // Top face
    if (!open) {
        glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
        glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
        glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
        glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    }

    if (!shaded) { glColor3f(0.2f, 0.2f, 1.0f); }
    // South face
    if (!open) {
        glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
        glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
        glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
        glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    }

    // North face
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    if (!shaded) { glColor3f(0.0f, 0.0f, 0.7f); }
    // West face
    if (!open) {
        glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
        glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
        glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
        glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    }

    // East face
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glEnd();

    glPopMatrix();
}

void ServerEntities::draw3DBox(const WFMath::Point<3> & coords,
                               const WFMath::AxisBox<3> & bbox)
{
    glPushMatrix();
    // origin();
    glTranslatef(coords.x(), coords.y(), coords.z());

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glEnd();

    glPopMatrix();

}

void ServerEntities::draw3DSelectedBox(const WFMath::Point<3> & coords,
                                       const WFMath::AxisBox<3> & bbox,
                                       float phase)
{
    glPushMatrix();
    // origin();
    glTranslatef(coords.x(), coords.y(), coords.z());

    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, m_antTexture);
    float xlen = phase + bbox.highCorner().x() - bbox.lowCorner().x();
    float ylen = phase + bbox.highCorner().y() - bbox.lowCorner().y();
    float zlen = phase + bbox.highCorner().z() - bbox.lowCorner().z();

    glBegin(GL_LINES);
    // glColor3f(0.0f, 0.0f, 1.0f);
    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(xlen);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(xlen);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glTexCoord1f(xlen);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glTexCoord1f(xlen);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(zlen);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(zlen);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(zlen);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(zlen);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(ylen);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.lowCorner().z());
    glTexCoord1f(ylen);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.lowCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.lowCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glTexCoord1f(ylen);
    glVertex3f(bbox.lowCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());

    glTexCoord1f(phase);
    glVertex3f(bbox.highCorner().x(),bbox.lowCorner().y(),bbox.highCorner().z());
    glTexCoord1f(ylen);
    glVertex3f(bbox.highCorner().x(),bbox.highCorner().y(),bbox.highCorner().z());
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_1D);

    glEnable(GL_DEPTH_TEST);
}

void ServerEntities::drawEntity(Eris::Entity* ent, entstack_t::const_iterator I)
{
    assert(ent != NULL);

    glPushMatrix();
    const WFMath::Point<3> & pos = ent->getPosition();
    glTranslatef(pos.x(), pos.y(), pos.z());
    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        assert(e != NULL);
        // debug(std::cout << ":" << e->getID() << e->getPosition() << ":"
                        // << e->getBBox().u << e->getBBox().v
                        // << std::endl << std::flush;);
        // if (!e->isVisible()) { continue; }
        bool openEntity = ((I != m_selectionStack.end()) && (*I == e));
        if (e->hasBBox()) {
            switch (m_renderMode) {
                case GlView::LINE:
                    draw3DBox(e->getPosition(), e->getBBox());
                    break;
                case GlView::SOLID:
                case GlView::SHADED:
                case GlView::TEXTURE:
                case GlView::SHADETEXT:
                default:
                    draw3DCube(e->getPosition(), e->getBBox(),
                               openEntity);
            }
            if ((e == m_selection) ||
                (m_selectionList.find(e) != m_selectionList.end())) {
                draw3DSelectedBox(e->getPosition(), e->getBBox());
            }
        } // else draw it without using its bbox FIXME how ?
        //draw3DBox(e->getPosition(), e->getBBox());
        if (openEntity) {
            entstack_t::const_iterator J = I;
            drawEntity(e, ++J);
        }
    }
    glPopMatrix();
}

void ServerEntities::drawWorld(Eris::Entity * wrld)
{
    assert(wrld != NULL);

    drawEntity(wrld, m_selectionStack.begin());
}

void ServerEntities::selectEntity(Eris::Entity* ent,entstack_t::const_iterator I)
{
    assert(ent != NULL);

    glPushMatrix();
    const WFMath::Point<3> & pos = ent->getPosition();
    glTranslatef(pos.x(), pos.y(), pos.z());
    int numEnts = ent->getNumMembers();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        assert(e != NULL);
        if (!e->isVisible()) { continue; }
        m_nameDict[++m_nameCount] = e;
        glPushName(m_nameCount);
        if (e->hasBBox()) {
            draw3DCube(e->getPosition(), e->getBBox());
        }
        if ((I != m_selectionStack.end()) && (*I == e)) {
            entstack_t::const_iterator J = I;
            selectEntity(e, ++J);
        }
        glPopName();
    }
}

bool ServerEntities::selectEntities(GlView & view,
                                    int nx, int ny, int fx, int fy, bool check)
{
    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    int sWidth = abs(fx - nx);
    int sHeight = abs(fy - ny);
    int sXCentre = (fx > nx) ? (nx + sWidth / 2) : (fx + sWidth / 2);
    int sYCentre = (fy > ny) ? (ny + sHeight / 2) : (fy + sHeight / 2);
    std::cout << "PICK: " << sXCentre << ":" << sYCentre << ":"
              << sWidth << ":" << sHeight << std::endl << std::flush;
    gluPickMatrix(sXCentre, sYCentre, sWidth, sHeight, viewport);

    view.setPickProjection(); // Sets the projection, sets up names
                              // and sets up the modelview

    m_nameCount = 0;
    m_nameDict.clear();

    entstack_t::const_iterator I = m_selectionStack.begin();

    Eris::Entity * root = m_serverConnection.world->getRootEntity();

    assert(root != NULL);

    selectEntity(root, I);

    
    int hits = glRenderMode(GL_RENDER);

    if (!check) {
        m_selectionList.clear();
        m_selection = NULL;
    }

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return false; }

    if (check && m_selectionList.empty()) { return false; }

    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        std::cout << "[";
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            std::cout << "{" << hitName << "}";
            entname_t::const_iterator I = m_nameDict.find(hitName);
            if (check) {
                if (m_selectionList.find(I->second) != m_selectionList.end()) {
                    std::cout << "SELECTION VERIFIED" << std::endl << std::flush;
                    std::cout << "Setting primart selection from "
                              << m_selection->getID() << " to "
                              << I->second->getID() << std::endl << std::flush;
                    m_selection = I->second;
                    return true;
                }
            } else {
                if (I != m_nameDict.end()) {
                    m_selection = I->second;
                    m_selectionList.insert(I->second);
                    // m_selectionStack[I->second] = 0;
                } else {
                    std::cout << "UNKNOWN NAME" << std::endl << std::flush;
                }
            }
        }
        std::cout << "]";
    }
    std::cout << std::endl << std::flush;

    return false;
}

void ServerEntities::newType(Eris::TypeInfo * node)
{
    assert(node != NULL);

    if (!m_gameEntityType->isBound()) {
        return;
    }
    if (!node->isA(m_gameEntityType)) {
        return;
    }
    Palette & p = m_model.m_mainWindow.m_palettewindow;
    p.addEntityEntry(&m_model, node->getName());
    m_model.typesAdded.emit();
}

void ServerEntities::descendTypeTree(Eris::TypeInfo * node)
{
    assert(node != NULL);

    m_model.m_mainWindow.m_palettewindow.addEntityEntry(&m_model, node->getName());
    const Eris::TypeInfoSet & children = node->getChildren();
    Eris::TypeInfoSet::const_iterator I = children.begin();
    for (; I != children.end(); I++) {
        descendTypeTree(*I);
    }
}

static const WFMath::Point<3> operator+(const WFMath::Point<3> & lhs,
                                 const WFMath::Point<3> & rhs)
{
    return WFMath::Point<3>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

void ServerEntities::alignEntityHeight(Eris::Entity * ent,
                                       const WFMath::Point<3> & o)
{
    assert(ent != NULL);

    const WFMath::Point<3> & pos = ent->getPosition();
    WFMath::Point<3> offset = pos + o;
    if (m_selectionList.find(ent) != m_selectionList.end()) {
        float x = offset.x();
        float y = offset.y();
        float z = o.z();
        float height = m_model.m_heightData.get(x, y) / 32.0f;
        std::cout << ent->getID() << " had height of " << offset.z()
                  << " and we change it to " << height << std::endl << std::flush;
        WFMath::Point<3> newPos = pos;
        newPos.z() = height - z;
        m_serverConnection.avatarMoveEntity(ent->getID(),
                                            ent->getContainer()->getID(),
                                            newPos, WFMath::Vector<3>(0,0,0));
    }
    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        assert(e != NULL);
        alignEntityHeight(e, offset);
    }
}

void ServerEntities::load(Gtk::FileSelection * fsel)
{
    std::string filename = fsel->get_filename();
    delete fsel;

    // m_model.updated.emit();
}

void ServerEntities::save(Gtk::FileSelection * fsel)
{
    std::string filename = fsel->get_filename();
    delete fsel;

    DummyDecoder d;
    std::fstream ios(filename.c_str(), std::ios::out);
    Atlas::Codecs::XML codec(ios, &d);
    Atlas::Message::Encoder e(&codec);

    e.StreamMessage(Atlas::Message::Object::MapType());
}

void ServerEntities::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

ServerEntities::ServerEntities(Model & model, Server & server) :
                               Layer(model, model.getName(), "ServerEntities"),
                               m_serverConnection(server),
                               m_selection(NULL), m_validDrag(false),
                               m_gameEntityType(NULL)
{
    Eris::TypeInfo::BoundType.connect(SigC::slot(this, &ServerEntities::newType));
    m_gameEntityType = Eris::TypeInfo::findSafe("game_entity");
    assert(m_gameEntityType != NULL);
    m_model.m_mainWindow.m_palettewindow.addModel(&m_model);
    if (m_gameEntityType->isBound()) {
        descendTypeTree(m_gameEntityType);
        m_model.typesAdded.emit();
    } else {
        std::cerr << "game_entity UNBOUND" << std::endl << std::flush;
    }
    
    /* observve the Eris world (in the future, we will need to get World* from
    the server object, when Eris supports multiple world objects */
    Eris::World::Instance()->EntityCreate.connect(
	SigC::slot(this, &ServerEntities::gotNewEntity)
    );
}

void ServerEntities::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Atlas Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &ServerEntities::load),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &ServerEntities::cancel),fsel));
    fsel->show();
}

void ServerEntities::exportFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Save Atlas Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &ServerEntities::save),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &ServerEntities::cancel),fsel));
    fsel->show();
}

void ServerEntities::moveTo(Eris::Entity * ent, Eris::Entity * world)
{
    if ((ent == world) || (ent == NULL)) {
        return;
    }
    Eris::Entity * cont = ent->getContainer();
    moveTo(cont, world);
    const WFMath::Point<3> & pos = ent->getPosition();
    glTranslatef(pos.x(), pos.y(), pos.z());
}

void ServerEntities::animate(GlView & view)
{
    if (m_selection == NULL) {
        return;
    }
    Eris::Entity * root = m_serverConnection.world->getRootEntity();
    glPushMatrix();
    moveTo(m_selection->getContainer(), root);
    draw3DSelectedBox(m_selection->getPosition(), m_selection->getBBox(),
                      view.getAnimCount());
    glPopMatrix();
}

void ServerEntities::draw(GlView & view)
{
    float winsize = std::max(view.width(), view.height());
    m_antTexture = view.getAnts();

    m_renderMode = view.getRenderMode();
    Eris::Entity * root = m_serverConnection.world->getRootEntity();
    drawWorld(root);
}

void ServerEntities::select(GlView & view, int x, int y)
{
    selectEntities(view, x, y, x + 1, y + 1);
}

void ServerEntities::select(GlView & view, int x, int y, int fx, int fy)
{
    selectEntities(view, x, y, fx, fy);
}

void ServerEntities::pushSelection()
{
    if (m_selection != NULL) {
        std::cout << "Pushing " << m_selection->getID() << std::endl << std::flush;
        m_selectionStack.push_back(m_selection);
        m_selection = NULL;
    }
}

void ServerEntities::popSelection()
{
    if (!m_selectionStack.empty()) {
        std::cout << "Popping" << std::endl << std::flush;
        m_selection = m_selectionStack.back();
        m_selectionStack.pop_back();
    }
}

void ServerEntities::dragStart(GlView & view, int x, int y)
{
    if (selectEntities(view, x, y, x + 1, y + 1, true)) {
        m_validDrag = true;
    }
}

void ServerEntities::dragUpdate(GlView & view, float x, float y, float z)
{
}

void ServerEntities::dragEnd(GlView & view, float x, float y, float z)
{
    if (m_validDrag) {
        std::cout << "MOVING " << m_selection->getID() << " to " << x
                  << ":" << y << ":" << z << std::endl << std::flush;
        m_serverConnection.avatarMoveEntity(m_selection->getID(),
                   m_selection->getContainer()->getID(),
                   m_selection->getPosition() + WFMath::Vector<3>(x, y, z));
    }
    m_validDrag = false;
}

void ServerEntities::insert(const WFMath::Point<3> & pos)
{
    const std::string & type = m_model.m_mainWindow.m_palettewindow.getCurrentEntity();
    std::cout << "INSERTING " << type << std::endl << std::flush;

    Atlas::Message::Object::MapType ent;
    ent["objtype"] = "object";
    ent["parents"] = Atlas::Message::Object::ListType(1, type);
    ent["loc"] = m_serverConnection.world->getRootEntity()->getID();
    ent["pos"] = pos.toAtlas();
    
    m_serverConnection.avatarCreateEntity(ent);
}

void ServerEntities::align(Alignment a)
{
    if (a != ALIGN_HEIGHT) { return; }

    Eris::Entity * root = m_serverConnection.world->getRootEntity();
    alignEntityHeight(root, WFMath::Point<3>(0,0,0));

}

void ServerEntities::gotNewEntity(Eris::Entity *ent)
{
    ent->Changed.connect(SigC::bind(
	SigC::slot(this, &ServerEntities::entityChanged),
	ent));
    ent->Moved.connect(SigC::slot(this, &ServerEntities::entityMoved));
    m_model.update(); // cause a re-draw
}

void ServerEntities::entityChanged(const Eris::StringSet &attrs, Eris::Entity *ent)
{
    m_model.update();	// a bit excessive I suppose
}

void ServerEntities::entityMoved(const WFMath::Point<3> &)
{
    m_model.update();	// a bit excessive I suppose
}

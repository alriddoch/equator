// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerEntities.h"
#include "GlView.h"
#include "Server.h"
#include "Model.h"
#include "MainWindow.h"
#include "Palette.h"

#include "debug.h"

#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>

#include <GL/glu.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

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
        if (e == m_selection) {
            draw3DSelectedBox(e->getPosition(), e->getBBox());
        }
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
        draw3DCube(e->getPosition(), e->getBBox());
        if ((I != m_selectionStack.end()) && (*I == e)) {
            entstack_t::const_iterator J = I;
            selectEntity(e, ++J);
        }
        glPopName();
    }
}

bool ServerEntities::selectSingleEntity(GlView & view,
                                        int nx, int ny, bool check)
{
    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix(nx, ny, 1, 1, viewport);

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

    if (check && (m_selection == NULL)) { return false; }

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
                if (m_selection == I->second) {
                    std::cout << "SELECTION VERIFIED" << std::endl << std::flush;
                    return true;
                }
            } else {
                if (I != m_nameDict.end()) {
                    m_selection = I->second;
                    m_selectionList[I->second] = 0;
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
    selectSingleEntity(view, x, y);
}

void ServerEntities::select(GlView & view, int x, int y, int w, int h)
{
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
    if (selectSingleEntity(view, x, y, true)) {
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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerEntities.h"

#include "GlView.h"
#include "Server.h"
#include "Model.h"
#include "MainWindow.h"
#include "Palette.h"
#include "WorldEntity.h"
#include "Terrain.h"

#include "gui/gtkmm/EntityExportOptions.h"
#include "gui/gtkmm/EntityImportOptions.h"

#include "common/debug.h"

#include "visual/TerrainRenderer.h"

#include <Mercator/Terrain.h>

#include <Eris/Connection.h>
#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>

#include <wfmath/atlasconv.h>
// #include <wfmath/ball.h>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Message/DecoderBase.h>

#include <GL/glu.h>

#include <gtkmm/fileselection.h>

#include <fstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;

EntityImportOptions * ServerEntities::m_importOptions = NULL;
EntityExportOptions * ServerEntities::m_exportOptions = NULL;


class DummyDecoder : public Atlas::Message::DecoderBase {
  private:
    virtual void objectArrived(const Atlas::Message::Element&) { }
  public:
    DummyDecoder() { }
};

class FileDecoder : public Atlas::Message::DecoderBase {
  private:
    std::fstream m_file;
    Atlas::Codecs::XML m_codec;
    Element::MapType m_entities;
    int m_count;
    std::string m_topLevelId;

    virtual void objectArrived(const Element & obj) {
        const Element::MapType & omap = obj.asMap();
        Element::MapType::const_iterator I;
        if (((I = omap.find("id")) == omap.end()) ||
            (!I->second.isString()) || (I->second.asString().empty())) {
            std::cerr << "WARNING: Object in file has no id. Not stored."
                      << std::endl << std::flush;
            return;
        }
        const std::string & id = I->second.asString();
        if (((I = omap.find("loc")) == omap.end()) ||
            (!I->second.isString()) || (I->second.asString().empty())) {
            // Entity with loc attribute must be the root entity
            if (!m_topLevelId.empty()) {
                std::cerr << "ERROR: File contained TLE with id = "
                          << m_topLevelId << " and also contains another id = "
                          << id << std::endl << std::flush;
            }
            m_topLevelId = id;
        }
        m_entities[id] = obj;
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
            m_codec.poll();
        }
    }

    void report() {
        std::cout << m_count << " objects read from file and stored."
                  << std::endl << std::flush;
        if (m_topLevelId.empty()) {
            std::cerr << "WARNING: No TLE found" << std::endl << std::flush;
        }
    }

    const Element::MapType & getEntities() const {
        return m_entities;
    }

    const std::string & getTopLevel() const {
        return m_topLevelId;
    }
};

void ServerEntities::draw3DCube(const WFMath::AxisBox<3> & bbox, bool open)
{
    glEnable(GL_CULL_FACE);
    if (open) {
        glCullFace(GL_FRONT);
    } else {
        glCullFace(GL_BACK);
    }
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };

    static const GLushort bottom_top[] = { 0, 3, 2, 1, 4, 5, 6, 7 };
    static const GLushort south_north[] = { 0, 1, 5, 4, 3, 7, 6, 2 };
    static const GLushort west_east[] = { 0, 4, 7, 3, 1, 2, 6, 5 };

    bool shaded = (m_renderMode == GlView::SHADED);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColor3f(0.0f, 0.0f, 1.0f);
    glDrawElements(GL_QUADS, 8, GL_UNSIGNED_SHORT, bottom_top);
    if (!shaded) { glColor3f(0.2f, 0.2f, 1.0f); }
    glDrawElements(GL_QUADS, 8, GL_UNSIGNED_SHORT, south_north);
    if (!shaded) { glColor3f(0.0f, 0.0f, 0.7f); }
    glDrawElements(GL_QUADS, 8, GL_UNSIGNED_SHORT, west_east);

    glDisable(GL_CULL_FACE);
}

void ServerEntities::draw3DBox(const WFMath::AxisBox<3> & bbox)
{
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLushort indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                                        3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, indices);
}

void ServerEntities::draw3DSelectedBox(GlView & view,
                                       const WFMath::AxisBox<3> & bbox,
                                       float phase)
{
    glDepthFunc(GL_LEQUAL);

    // if (phase != 0.f) {
    std::cout << "phase " << phase << std::endl << std::flush;
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glTranslatef(phase, phase, phase);
        glMatrixMode(GL_MODELVIEW);
    // }

    GLfloat scale = 0.5f * view.getScale();
    GLfloat sx[] = { scale, scale, scale, 0 };

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, m_antTexture);
    glColor3f(1.f, 1.f, 1.f);
    glEnable(GL_TEXTURE_GEN_S);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx);
    
    GLfloat vertices[] = {
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.lowCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.lowCorner().z(),
        bbox.lowCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.lowCorner().y(), bbox.highCorner().z(),
        bbox.highCorner().x(), bbox.highCorner().y(), bbox.highCorner().z(),
        bbox.lowCorner().x(), bbox.highCorner().y(), bbox.highCorner().z() 
    };
    static const GLushort indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                                        3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, indices);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_1D);

    glDepthFunc(GL_LESS);

    // if (phase != 0.f) {
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    // }

}

void ServerEntities::orient(const WFMath::Quaternion & orientation)
{
    if (!orientation.isValid()) {
        return;
    }
    float orient[4][4];
    WFMath::RotMatrix<3> omatrix(orientation); // .asMatrix(orient);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            orient[i][j] = omatrix.elem(i,j);
        }
    }
    orient[3][0] = orient[3][1] = orient[3][2] = orient[0][3] = orient[1][3] = orient[2][3] = 0.0f;
    orient[3][3] = 1.0f;
    glMultMatrixf(&orient[0][0]);
}


void ServerEntities::drawEntity(GlView & view, Eris::Entity* ent,
                                Eris::Entity* pe,
                                entstack_t::const_iterator I)
{
    assert(ent != 0);

    PosType pos = ent->getPosition();

    // This is where code would go to adjust position for velocity etc.

    // Manipulate camera position if necessary

    // Check type of entity to see if it should be rendered

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());

    bool openEntity = ((ent == m_world) ||
                       ((I != m_selectionStack.end()) && (*I == ent)));

    if (ent->hasBBox()) {
        switch (m_renderMode) {
            case GlView::LINE:
                draw3DBox(ent->getBBox());
                break;
            case GlView::SOLID:
            case GlView::SHADED:
            case GlView::TEXTURE:
            case GlView::SHADETEXT:
            default:
                draw3DCube(ent->getBBox(), openEntity);
        }
        if ((ent == m_selection) ||
            (m_selectionList.find(ent) != m_selectionList.end())) {
            draw3DSelectedBox(view, ent->getBBox());
        }
    } // else draw it without using its bbox FIXME how ?

    if (openEntity) {
        int numEnts = ent->getNumMembers();
        entstack_t::const_iterator J = I;
        if (ent != m_world) {
            ++J;
        }
        debug(std::cout << ent->getID() << " " << numEnts << " emts"
                        << std::endl << std::flush;);
        for (int i = 0; i < numEnts; i++) {
            Eris::Entity * e = ent->getMember(i);
            assert(e != NULL);

            drawEntity(view, e, 0, J);
        }
    }

    glPopMatrix();
}

void ServerEntities::drawWorld(GlView & view, Eris::Entity * wrld)
{
    assert(wrld != NULL);

    m_world = wrld;

    drawEntity(view, wrld, 0, m_selectionStack.begin());
}

void ServerEntities::selectEntity(Eris::Entity * wrld,
                                  Eris::Entity * ent,
                                  entstack_t::const_iterator I)
{
    assert(ent != NULL);

    PosType pos = ent->getPosition();

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());

    if (ent->hasBBox()) {
        draw3DCube(ent->getBBox());
    }

    if ((ent == wrld) || (I != m_selectionStack.end()) && (*I == ent)) {
        int numEnts = ent->getNumMembers();
        entstack_t::const_iterator J = I;
        if (ent != wrld) {
            ++J;
        }
        for (int i = 0; i < numEnts; i++) {
            Eris::Entity * e = ent->getMember(i);
            assert(e != NULL);

            m_nameDict[++m_nameCount] = e;
            glPushName(m_nameCount);
            selectEntity(wrld, e, J);
            glPopName();
        }
    }
    glPopMatrix();
}

bool ServerEntities::selectEntities(GlView & view,
                                    int nx, int ny, int fx, int fy, bool check)
{
    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

    m_nameCount = 0;
    m_nameDict.clear();

    entstack_t::const_iterator I = m_selectionStack.begin();

    Eris::Entity * root = m_serverConnection.m_world->getRootEntity();

    assert(root != NULL);

    selectEntity(root, root, I);

    
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
                    view.startAnimation();
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
    std::cout << "GOT NEW SERVER TYPE" << std::endl << std::flush;
    assert(node != NULL);

    if (!m_gameEntityType->isBound()) {
        return;
    }
    if (!node->isA(m_gameEntityType)) {
        return;
    }
    std::cout << "GOT NEW SERVER GAME ENTITY TYPE" << std::endl << std::flush;
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

void ServerEntities::alignEntityParent(Eris::Entity * ent)
{
    assert(ent != NULL);
    std::cout << "Aligning ... " << std::endl << std::flush;

    Eris::Entity * parent = ent->getContainer();
    if ((parent != 0) && (m_selectionList.find(ent) != m_selectionList.end())) {
        TerrainEntity * tparent = dynamic_cast<TerrainEntity *>(parent);
        if (tparent != 0) {
            EntityRenderer * er = tparent->m_drawer;
            if (er != 0) {
                std::cout << "Terrain ... " << std::endl << std::flush;
                TerrainRenderer * tr = dynamic_cast<TerrainRenderer *>(er);
                if (tr != 0) {
                    PosType pos = ent->getPosition();
                    float height = tr->m_terrain.get(pos.x(), pos.y());
                    std::cout << ent->getID() << " had height of " << pos.z()
                              << " and we change it to terrain level "
                              << height << std::endl << std::flush;
                    pos.z() = height;
                    m_serverConnection.avatarMoveEntity(ent->getID(),
                                                ent->getContainer()->getID(),
                                                pos, VelType(0,0,0));
                }
                
            } else {
                std::cout << "No renderer ... " << std::endl << std::flush;
            }
        } else {
            std::cout << "Not terrain ... " << std::endl << std::flush;
            PosType pos = ent->getPosition();
            float height = 0.f;
            std::cout << ent->getID() << " had height of " << pos.z()
                      << " and we change it to zero " << height << std::endl << std::flush;
            pos.z() = height;
            m_serverConnection.avatarMoveEntity(ent->getID(),
                                        ent->getContainer()->getID(),
                                        pos, VelType(0,0,0));
        }
#warning FIXME - get the server object to handle the adjustment.
        // The server object knows about what data we have available.
        // We should probably support alignments like "other layer", "parent"
    }
    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        assert(e != NULL);
        alignEntityParent(e);
    }
}

void ServerEntities::loadOptions(Gtk::FileSelection * fsel)
{
    m_loadOptionsDone.disconnect();
    m_loadOptionsDone = m_importOptions->m_ok->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::load), fsel));
    m_importOptions->show_all();
    fsel->hide();
}

void ServerEntities::saveOptions(Gtk::FileSelection * fsel)
{
    m_saveOptionsDone.disconnect();
    m_saveOptionsDone = m_exportOptions->m_ok->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::save), fsel));
    m_exportOptions->show_all();
    fsel->hide();
}

void ServerEntities::insertEntityContents(const std::string & container,
                     const Atlas::Message::Element::MapType & ent,
                     const Atlas::Message::Element::MapType & entities)
{
    Atlas::Message::Element::MapType::const_iterator I = ent.find("contains");
    if ((I == ent.end()) || !I->second.isList() || I->second.asList().empty()) {
        return;
    }
    const Atlas::Message::Element::ListType & contents = I->second.asList();
    Atlas::Message::Element::ListType::const_iterator J = contents.begin();
    for(; J != contents.end(); ++J) {
        if (!J->isString()) { continue; }
        Atlas::Message::Element::MapType::const_iterator K = entities.find(J->asString());
        if (K == entities.end()) { continue; }
        Atlas::Message::Element::MapType newEnt = K->second.asMap();
        newEnt["loc"] = container;
        newEnt.erase("id");
        m_serverConnection.avatarCreateEntity(newEnt);
        // So the problem here is, we can't just stuff the world full of
        // entities while we sit here and do nothing, output from the
        // server will get really backed up, and we also need to solve the
        // problem of how we handle the contents of this entity
    }
}

void ServerEntities::load(Gtk::FileSelection * fsel)
{
    std::string filename = fsel->get_filename();
    delete fsel;

    FileDecoder fd(filename);
    fd.read();
    fd.report();
    const Atlas::Message::Element::MapType & fileEnts = fd.getEntities();
    const std::string & topId = fd.getTopLevel();
    Atlas::Message::Element::MapType::const_iterator I = fileEnts.find(topId);
    if (I == fileEnts.end()) {
        std::cerr << "ERROR: Failed to find top level entity " << topId << " in file"
                  << std::endl << std::flush;
        return;
    } else {
        std::cerr << "Top level entity " << topId << " found in file"
                  << std::endl << std::flush;
    }
    if (m_importOptions->m_target == EntityImportOptions::IMPORT_TOPLEVEL) {
        insertEntityContents(m_serverConnection.m_world->getRootEntity()->getID(),
                             I->second.asMap(), fileEnts);
    } else /* (m_importOptions->m_target == IMPORT_SELECTION) */ {
        if (m_selection == NULL) {
            std::cerr << "ERROR: Can't import into selection, as nothing is selected. Tell the user" << std::endl << std::flush;
            return;
        }
        insertEntityContents(m_selection->getID(),
                             I->second.asMap(), fileEnts);
    }
    // m_model.updated.emit();
}

void ServerEntities::exportEntity(const std::string & id,
                                  Atlas::Message::Encoder & e,
                                  Eris::Entity * ee)
{
    Atlas::Message::Element::MapType ent;
    Atlas::Message::Element::ListType contents;
    ent["id"] = id;
    unsigned int numEnts = ee->getNumMembers();
    for(unsigned int i = 0; i < numEnts; ++i) {
        Eris::Entity * me = ee->getMember(i);
        std::string eid = me->getID();
        if (m_exportOptions->m_charCheck->get_active() &&
            m_exportOptions->m_charType != NULL) {
            Eris::TypeInfoPtr entType = me->getType();
            if (entType != NULL && entType->isA(m_exportOptions->m_charType)) {
                std::cout << "Omitting " << eid << std::endl << std::flush;
                continue;
            }
        }
        if (m_exportOptions->m_appendCheck->get_active()) {
            eid += m_exportOptions->m_idSuffix->get_text();
        }
        exportEntity(eid, e, me);
        contents.push_back(eid);
    }
    ent["contains"] = contents;
    ent["pos"] = ee->getPosition().toAtlas();
    ent["bbox"] = ee->getBBox().toAtlas();
    ent["orientation"] = ee->getOrientation().toAtlas();
    ent["name"] = ee->getName();
    ent["parents"] = Atlas::Message::Element::ListType(1, *ee->getInherits().begin());
    Eris::Entity * loc = ee->getContainer();
    if (loc != NULL) {
        // FIXME AJR 2002-07-15
        // The id of the container actually needs a little bit of processing.
        // If we have overriddent the top level container, it needs to be
        // that, otherwise it might need the suffix appended
        ent["loc"] = loc->getID();
    }
    e.streamMessage(ent);
}

void ServerEntities::save(Gtk::FileSelection * fsel)
{
    std::string filename = fsel->get_filename();
    delete fsel;

    Eris::Entity * export_root = NULL;
    switch (m_exportOptions->m_target) {
        case EntityExportOptions::EXPORT_SELECTION:
            export_root = m_selection;
            break;
        case EntityExportOptions::EXPORT_ALL_SELECTED:
            if (m_selection != 0) {
                export_root = m_selection->getContainer();
            }
            break;
        case EntityExportOptions::EXPORT_ALL:
        case EntityExportOptions::EXPORT_VISIBLE:
        default:
            export_root = m_serverConnection.m_world->getRootEntity();
            break;
    }

    if (export_root == NULL) {
        std::cerr << "ERROR: Nothing to export" << std::endl << std::flush;
        return;
    }

    DummyDecoder d;
    std::fstream ios(filename.c_str(), std::ios::out);
    Atlas::Codecs::XML codec(ios, &d);
    Atlas::Message::Encoder e(&codec);

    codec.streamBegin();

    std::string exportedRootId;
    if (m_exportOptions->m_setRootCheck->get_active()) {
        exportedRootId = m_exportOptions->m_rootId->get_text();
    } else {
        exportedRootId = export_root->getID();
        if (m_exportOptions->m_appendCheck->get_active()) {
            exportedRootId += m_exportOptions->m_idSuffix->get_text();
        }
    }

    if (m_exportOptions->m_charCheck->get_active()) {
        m_exportOptions->m_charType = m_serverConnection.m_connection.getTypeService()->getTypeByName("character");
    }

    if (m_exportOptions->m_target == EntityExportOptions::EXPORT_ALL_SELECTED) {
        Atlas::Message::Element::MapType ent;
        Atlas::Message::Element::ListType contents;
        ent["id"] = exportedRootId;
        unsigned int numEnts = export_root->getNumMembers();
        for(unsigned int i = 0; i < numEnts; ++i) {
            Eris::Entity * me = export_root->getMember(i);
            if (m_selectionList.find(me) == m_selectionList.end()) {continue;}
            std::string eid = me->getID();
            if (m_exportOptions->m_appendCheck->get_active()) {
                eid += m_exportOptions->m_idSuffix->get_text();
            }
            exportEntity(eid, e, me);
            contents.push_back(eid);
        }
        ent["contains"] = contents;
        ent["pos"] = export_root->getPosition().toAtlas();
        ent["bbox"] = export_root->getBBox().toAtlas();
        ent["orientation"] = export_root->getOrientation().toAtlas();
        ent["name"] = export_root->getName();
        ent["parents"] = Atlas::Message::Element::ListType(1, *export_root->getInherits().begin());
        e.streamMessage(ent);
    } else {
        exportEntity(exportedRootId, e, export_root);
    }

    codec.streamEnd();
}

void ServerEntities::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void ServerEntities::createOptionsWindows()
{
    m_importOptions = new EntityImportOptions();
    m_exportOptions = new EntityExportOptions();
}

void ServerEntities::connectEntity(Eris::Entity * ent)
{
    ent->Changed.connect(SigC::bind(
        SigC::slot(*this, &ServerEntities::entityChanged),
        ent));
    ent->Moved.connect(SigC::slot(*this, &ServerEntities::entityMoved));

    int numEnts = ent->getNumMembers();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        connectEntity(e);
    }
}

ServerEntities::ServerEntities(Model & model, Server & server) :
                               Layer(model, model.getName(), "ServerEntities"),
                               m_serverConnection(server),
                               m_selection(NULL), m_validDrag(false),
                               m_gameEntityType(NULL)
{
    // FIXME This stuff populates palette, but does not belong here.
    // It probably belongs in server.
    m_serverConnection.m_connection.getTypeService()->BoundType.connect(SigC::slot(*this, &ServerEntities::newType));
    m_gameEntityType = m_serverConnection.m_connection.getTypeService()->getTypeByName("game_entity");
    assert(m_gameEntityType != NULL);
    m_model.m_mainWindow.m_palettewindow.addModel(&m_model);
    if (m_gameEntityType->isBound()) {
        descendTypeTree(m_gameEntityType);
        m_model.typesAdded.emit();
    } else {
        std::cerr << "game_entity UNBOUND" << std::endl << std::flush;
    }
    
    Eris::Entity * worldRoot = m_serverConnection.m_world->getRootEntity();

    connectEntity(worldRoot);

    // observe the Eris world (in the future, we will need to get World* from
    // the server object, when Eris supports multiple world objects
    m_serverConnection.m_world->EntityCreate.connect(
        SigC::slot(*this, &ServerEntities::gotNewEntity)
    );

    if (m_importOptions == NULL) {
        createOptionsWindows();
    }
}

void ServerEntities::options()
{
}

void ServerEntities::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Atlas Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::loadOptions),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::cancel),fsel));
    fsel->show();
}

void ServerEntities::exportFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Save Atlas Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::saveOptions),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::cancel),fsel));
    fsel->show();
}

void ServerEntities::selectInvert()
{
}

void ServerEntities::selectAll()
{
}

void ServerEntities::selectNone()
{
    m_selection = 0;
    m_selectionList.clear();
    m_selectionStack.clear();
}

void ServerEntities::moveTo(Eris::Entity * ent, Eris::Entity * world)
{
    if ((ent == world) || (ent == NULL)) {
        return;
    }
    Eris::Entity * cont = ent->getContainer();
    moveTo(cont, world);
    PosType pos = ent->getPosition();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(ent->getOrientation());
}

bool ServerEntities::animate(GlView & view)
{
    if (m_selection == NULL) {
        return false;
    }
    Eris::Entity * root = m_serverConnection.m_world->getRootEntity();
    glPushMatrix();
    moveTo(m_selection, root);
    draw3DSelectedBox(view, m_selection->getBBox(),
                      view.getAnimCount());
    glPopMatrix();
    return true;
}

void ServerEntities::draw(GlView & view)
{
    m_antTexture = view.getAnts();

    m_renderMode = view.getRenderMode(m_name);
    Eris::Entity * root = m_serverConnection.m_world->getRootEntity();
    drawWorld(view, root);
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
    m_dragX = x; m_dragY = y; m_dragZ = z;
}

void ServerEntities::dragEnd(GlView & view, float x, float y, float z)
{
    if (m_validDrag) {
        std::cout << "MOVING " << m_selection->getID() << " to " << x
                  << ":" << y << ":" << z << std::endl << std::flush;
        m_serverConnection.avatarMoveEntity(m_selection->getID(),
                   m_selection->getContainer()->getID(),
                   m_selection->getPosition() + VelType(x, y, z));
    }
    m_validDrag = false;
}

void ServerEntities::insert(const PosType & pos)
{
    const std::string & type = m_model.m_mainWindow.m_palettewindow.getCurrentEntity();
    std::cout << "INSERTING " << type << std::endl << std::flush;

    Atlas::Message::Element::MapType ent;
    ent["objtype"] = "object";
    ent["parents"] = Atlas::Message::Element::ListType(1, type);
    ent["loc"] = m_serverConnection.m_world->getRootEntity()->getID();
    ent["pos"] = pos.toAtlas();
    
    m_serverConnection.avatarCreateEntity(ent);
}

void ServerEntities::align(Alignment a)
{
    if (a != ALIGN_PARENT) { return; }

    Eris::Entity * root = m_serverConnection.m_world->getRootEntity();
    alignEntityParent(root);

}

void ServerEntities::gotNewEntity(Eris::Entity *ent)
{
    ent->Changed.connect(SigC::bind(
        SigC::slot(*this, &ServerEntities::entityChanged),
        ent));
    ent->Moved.connect(SigC::slot(*this, &ServerEntities::entityMoved));
    m_model.update(); // cause a re-draw
}

void ServerEntities::entityChanged(const Eris::StringSet &attrs, Eris::Entity *ent)
{
    m_model.update();   // a bit excessive I suppose
}

void ServerEntities::entityMoved(const PosType &)
{
    m_model.update();   // a bit excessive I suppose
}

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

#include <Mercator/Terrain.h>

#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/TypeInfo.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Codecs/XML.h>

#include <GL/glu.h>

#include <gtkmm/fileselection.h>
#include <gtkmm/frame.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/label.h>

#include <fstream>

static const bool debug_flag = false;

using Atlas::Message::Object;

ImportOptions * ServerEntities::m_importOptions = NULL;
ExportOptions * ServerEntities::m_exportOptions = NULL;


class DummyDecoder : public Atlas::Message::DecoderBase {
  private:
    virtual void ObjectArrived(const Atlas::Message::Object&) { }
  public:
    DummyDecoder() { }
};

class FileDecoder : public Atlas::Message::DecoderBase {
  private:
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

    const Object::MapType & getEntities() const {
        return m_entities;
    }

    const std::string & getTopLevel() const {
        return m_topLevelId;
    }
};

class ImportOptions : public Gtk::Window
{
  private:
    
  public:
    typedef enum import_target { IMPORT_TOPLEVEL, IMPORT_SELECTION } ImportTarget;

    Gtk::Button * m_ok;
    ImportTarget m_target;

    ImportOptions() : m_target(IMPORT_TOPLEVEL) {
        Gtk::VBox * vbox = manage( new Gtk::VBox() );

        Gtk::Frame * frame = manage( new Gtk::Frame("Import entities into..") );
        Gtk::HBox * hbox = manage( new Gtk::HBox() );
        Gtk::RadioButton * rb1 = manage( new Gtk::RadioButton("top level") );
        rb1->signal_clicked().connect(SigC::bind<ImportTarget>(slot(*this, &ImportOptions::setImportTarget),IMPORT_TOPLEVEL));
        hbox->pack_start(*rb1, false, false, 2);
        Gtk::RadioButton::Group rgp = rb1->get_group();
        Gtk::RadioButton * rb = manage( new Gtk::RadioButton(rgp, "selected entity") );
        rb->signal_clicked().connect(SigC::bind<ImportTarget>(slot(*this, &ImportOptions::setImportTarget),IMPORT_SELECTION));
        hbox->pack_start(*rb, false, false, 2);
        frame->add(*hbox);
        vbox->pack_start(*frame, false, false, 2);

        hbox = manage( new Gtk::HBox() );
        m_ok = manage( new Gtk::Button("OK") );
        hbox->pack_start(*m_ok, true, true, 2);
        Gtk::Button * b = manage( new Gtk::Button("Cancel") );
        b->signal_clicked().connect(slot(*this, &ImportOptions::cancel));
        hbox->pack_start(*b, true, true, 2);
        vbox->pack_end(*hbox, false, false, 2);
        add(*vbox);
    }

    void setImportTarget(ImportTarget it) {
        m_target = it;
    }

    void cancel() {
        hide();
    }
};

class ExportOptions : public Gtk::Window
{
  private:
    
  public:
    typedef enum export_target { EXPORT_ALL, EXPORT_VISIBLE, EXPORT_SELECTION, EXPORT_ALL_SELECTED } ExportTarget;

    Gtk::Button * m_ok;
    Gtk::CheckButton * m_charCheck; 
    Gtk::CheckButton * m_appendCheck; 
    Gtk::Entry * m_idSuffix;
    Gtk::CheckButton * m_setRootCheck; 
    Gtk::Entry * m_rootId;
    ExportTarget m_target;
    Eris::TypeInfoPtr m_charType;

    ExportOptions() : m_target(EXPORT_ALL), m_charType(NULL) {
        Gtk::VBox * vbox = manage( new Gtk::VBox() );

        Gtk::Frame * frame = manage( new Gtk::Frame("Export..") );
        Gtk::VBox * rbox = manage( new Gtk::VBox() );
        Gtk::RadioButton * rb1 = manage( new Gtk::RadioButton("all entities") );
        rb1->signal_clicked().connect(SigC::bind<ExportTarget>(slot(*this, &ExportOptions::setExportTarget),EXPORT_ALL));
        rbox->pack_start(*rb1, false, false, 2);
        Gtk::RadioButton::Group rgp = rb1->get_group();
        Gtk::RadioButton * rb = manage( new Gtk::RadioButton(rgp, "visible entities") );
        rb->signal_clicked().connect(SigC::bind<ExportTarget>(slot(*this, &ExportOptions::setExportTarget),EXPORT_VISIBLE));
        rbox->pack_start(*rb, false, false, 2);
        rb = manage( new Gtk::RadioButton(rgp, "selected entity") );
        rb->signal_clicked().connect(SigC::bind<ExportTarget>(slot(*this, &ExportOptions::setExportTarget),EXPORT_SELECTION));
        rbox->pack_start(*rb, false, false, 2);
        rb = manage( new Gtk::RadioButton(rgp, "all selected entities") );
        rb->signal_clicked().connect(SigC::bind<ExportTarget>(slot(*this, &ExportOptions::setExportTarget),EXPORT_ALL_SELECTED));
        rbox->pack_start(*rb, false, false, 2);
        m_charCheck = manage( new Gtk::CheckButton("Remove characters") );
        rbox->pack_start(*m_charCheck, false, false, 0);
        
        frame->add(*rbox);
        vbox->pack_start(*frame, false, false, 2);

        frame = manage( new Gtk::Frame("ID handling") );
        Gtk::VBox * lvbox = manage( new Gtk::VBox() );
        Gtk::HBox * hbox = manage( new Gtk::HBox() );
        m_appendCheck = manage( new Gtk::CheckButton("Append") );
        hbox->pack_start(*m_appendCheck, false, false, 0);
        m_idSuffix = manage( new Gtk::Entry() );
        m_idSuffix->set_text("_map");
        hbox->pack_start(*m_idSuffix, false, false, 0);
        Gtk::Label * t = manage( new Gtk::Label("to IDs in file.") );
        hbox->pack_end(*t, false, false, 0);
        lvbox->pack_start(*hbox, false, false, 2);

        hbox = manage( new Gtk::HBox() );
        m_setRootCheck = manage( new Gtk::CheckButton("Set root id to ") );
        hbox->pack_start(*m_setRootCheck, false, false, 0);
        m_rootId = manage( new Gtk::Entry() );
        m_rootId->set_text("world_0");
        hbox->pack_start(*m_rootId, false, false, 0);
        t = manage( new Gtk::Label(".") );
        hbox->pack_end(*t, false, false, 0);
        lvbox->pack_start(*hbox, false, false, 2);

        frame->add(*lvbox);
        vbox->pack_start(*frame, false, false, 2);

        hbox = manage( new Gtk::HBox() );
        m_ok = manage( new Gtk::Button("OK") );
        hbox->pack_start(*m_ok, true, true, 2);
        Gtk::Button * b = manage( new Gtk::Button("Cancel") );
        b->signal_clicked().connect(slot(*this, &ExportOptions::cancel));
        hbox->pack_start(*b, true, true, 2);

        vbox->pack_start(*hbox, false, false, 2);
        add(*vbox);
    }

    void setExportTarget(ExportTarget et) {
        m_target = et;
    }

    void cancel() {
        hide();
    }
};

void ServerEntities::draw3DCube(const WFMath::Point<3> & coords,
                                const WFMath::AxisBox<3> & bbox, bool open)
{
    bool shaded = (m_renderMode == GlView::SHADED);
    glPushMatrix();

    glTranslatef(coords.x(), coords.y(), coords.z());

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

    GLuint bottom[] = { 0, 1, 2, 3 };
    GLuint top[] = { 4, 5, 6, 7 };
    GLuint south[] = { 0, 1, 5, 4 };
    GLuint north[] = { 3, 2, 6, 7 };
    GLuint west[] = { 0, 3, 7, 4 };
    GLuint east[] = { 1, 2, 6, 5 };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColor3f(0.0f, 0.0f, 1.0f);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, bottom);
    if (!open) {
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, top);
    }
    if (!shaded) { glColor3f(0.2f, 0.2f, 1.0f); }
    if (!open) {
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, south);
    }
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, north);
    if (!shaded) { glColor3f(0.0f, 0.0f, 0.7f); }
    if (!open) {
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, west);
    }
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, east);
    glPopMatrix();
}

void ServerEntities::draw3DBox(const WFMath::Point<3> & coords,
                               const WFMath::AxisBox<3> & bbox)
{
    glPushMatrix();

    glTranslatef(coords.x(), coords.y(), coords.z());

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
    GLuint indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                         3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };

    glColor3f(0.0f, 0.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
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
    GLuint indices[] = { 0, 1, 3, 2, 7, 6, 4, 5, 0, 4, 1, 5,
                         3, 7, 2, 6, 0, 3, 1, 2, 4, 7, 5, 6 };
    // FIXME THis is all wrong
    GLfloat texcoords[] = { phase, xlen, phase, xlen, phase, xlen, phase, xlen,
                            phase, zlen, phase, zlen, phase, zlen, phase, zlen,
                            phase, ylen, phase, ylen, phase, ylen, phase, ylen };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(1, GL_FLOAT, 0, texcoords);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    glDisable(GL_TEXTURE_1D);

    glEnable(GL_DEPTH_TEST);
}

void ServerEntities::drawEntity(Eris::Entity* ent, entstack_t::const_iterator I)
{
    assert(ent != NULL);

    glPushMatrix();
    WFMath::Point<3> pos = ent->getPosition();
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
    WFMath::Point<3> pos = ent->getPosition();
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

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

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

static const WFMath::Point<3> operator+(const WFMath::Point<3> & lhs,
                                 const WFMath::Point<3> & rhs)
{
    return WFMath::Point<3>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

void ServerEntities::alignEntityHeight(Eris::Entity * ent,
                                       const WFMath::Point<3> & o)
{
    assert(ent != NULL);

    WFMath::Point<3> pos = ent->getPosition();
    WFMath::Point<3> offset = pos + o;
    if (m_selectionList.find(ent) != m_selectionList.end()) {
        float x = offset.x();
        float y = offset.y();
        float z = o.z();
        float height = m_model.m_terrain.get(x, y); // / 32.0f;
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

void ServerEntities::loadOptions(Gtk::FileSelection * fsel)
{
    m_loadOptionsDone.disconnect();
    m_loadOptionsDone = m_importOptions->m_ok->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::load), fsel));
    m_importOptions->show_all();
}

void ServerEntities::saveOptions(Gtk::FileSelection * fsel)
{
    m_saveOptionsDone = m_exportOptions->m_ok->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &ServerEntities::save), fsel));
    m_exportOptions->show_all();
}

void ServerEntities::insertEntityContents(const std::string & container,
                     const Atlas::Message::Object::MapType & ent,
                     const Atlas::Message::Object::MapType & entities)
{
    Atlas::Message::Object::MapType::const_iterator I = ent.find("contains");
    if ((I == ent.end()) || !I->second.IsList() || I->second.AsList().empty()) {
        return;
    }
    const Atlas::Message::Object::ListType & contents = I->second.AsList();
    Atlas::Message::Object::ListType::const_iterator J = contents.begin();
    for(; J != contents.end(); ++J) {
        if (!J->IsString()) { continue; }
        Atlas::Message::Object::MapType::const_iterator K = entities.find(J->AsString());
        if (K == entities.end()) { continue; }
        Atlas::Message::Object::MapType newEnt = K->second.AsMap();
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
    m_importOptions->hide();

    std::string filename = fsel->get_filename();
    delete fsel;

    FileDecoder fd(filename);
    fd.read();
    fd.report();
    const Atlas::Message::Object::MapType & fileEnts = fd.getEntities();
    const std::string & topId = fd.getTopLevel();
    Atlas::Message::Object::MapType::const_iterator I = fileEnts.find(topId);
    if (I == fileEnts.end()) {
        std::cerr << "ERROR: Failed to find top level entity " << topId << " in file"
                  << std::endl << std::flush;
        return;
    } else {
        std::cerr << "Top level entity " << topId << " found in file"
                  << std::endl << std::flush;
    }
    if (m_importOptions->m_target == ImportOptions::IMPORT_TOPLEVEL) {
        insertEntityContents(m_serverConnection.world->getRootEntity()->getID(),
                             I->second.AsMap(), fileEnts);
    } else /* (m_importOptions->m_target == IMPORT_SELECTION) */ {
        if (m_selection == NULL) {
            std::cerr << "ERROR: Can't import into selection, as nothing is selected. Tell the user" << std::endl << std::flush;
            return;
        }
        insertEntityContents(m_selection->getID(),
                             I->second.AsMap(), fileEnts);
    }
    // m_model.updated.emit();
}

void ServerEntities::exportEntity(const std::string & id,
                                  Atlas::Message::Encoder & e,
                                  Eris::Entity * ee)
{
    Atlas::Message::Object::MapType ent;
    Atlas::Message::Object::ListType contents;
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
    ent["parents"] = Atlas::Message::Object::ListType(1, *ee->getInherits().begin());
    Eris::Entity * loc = ee->getContainer();
    if (loc != NULL) {
        // FIXME AJR 2002-07-15
        // The id of the container actually needs a little bit of processing.
        // If we have overriddent the top level container, it needs to be
        // that, otherwise it might need the suffix appended
        ent["loc"] = loc->getID();
    }
    e.StreamMessage(ent);
}

void ServerEntities::save(Gtk::FileSelection * fsel)
{
    m_exportOptions->hide();

    std::string filename = fsel->get_filename();
    delete fsel;

    Eris::Entity * export_root = NULL;
    switch (m_exportOptions->m_target) {
        case ExportOptions::EXPORT_SELECTION:
            export_root = m_selection;
        case ExportOptions::EXPORT_ALL_SELECTED:
            if (m_selection != NULL) {
                export_root = m_selection->getContainer();
            }
        case ExportOptions::EXPORT_ALL:
        case ExportOptions::EXPORT_VISIBLE:
        default:
            export_root = m_serverConnection.world->getRootEntity();
    }

    if (export_root == NULL) {
        std::cerr << "ERROR: Nothing to export" << std::endl << std::flush;
        return;
    }

    DummyDecoder d;
    std::fstream ios(filename.c_str(), std::ios::out);
    Atlas::Codecs::XML codec(ios, &d);
    Atlas::Message::Encoder e(&codec);

    codec.StreamBegin();

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
        m_exportOptions->m_charType = m_serverConnection.connection.getTypeInfoEngine()->findSafe("character");
    }

    if (m_exportOptions->m_target == ExportOptions::EXPORT_ALL_SELECTED) {
        Atlas::Message::Object::MapType ent;
        Atlas::Message::Object::ListType contents;
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
        ent["parents"] = Atlas::Message::Object::ListType(1, *export_root->getInherits().begin());
        e.StreamMessage(ent);
    } else {
        exportEntity(exportedRootId, e, export_root);
    }

    codec.StreamEnd();
}

void ServerEntities::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void ServerEntities::createOptionsWindows()
{
    m_importOptions = new ImportOptions();
    m_exportOptions = new ExportOptions();
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
    m_serverConnection.connection.getTypeInfoEngine()->BoundType.connect(SigC::slot(*this, &ServerEntities::newType));
    m_gameEntityType = m_serverConnection.connection.getTypeInfoEngine()->findSafe("game_entity");
    assert(m_gameEntityType != NULL);
    m_model.m_mainWindow.m_palettewindow.addModel(&m_model);
    if (m_gameEntityType->isBound()) {
        descendTypeTree(m_gameEntityType);
        m_model.typesAdded.emit();
    } else {
        std::cerr << "game_entity UNBOUND" << std::endl << std::flush;
    }
    
    connectEntity(m_serverConnection.world->getRootEntity());
    /* observve the Eris world (in the future, we will need to get World* from
    the server object, when Eris supports multiple world objects */
    m_serverConnection.world->EntityCreate.connect(
        SigC::slot(*this, &ServerEntities::gotNewEntity)
    );

    if (m_importOptions == NULL) {
        createOptionsWindows();
    }
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

void ServerEntities::moveTo(Eris::Entity * ent, Eris::Entity * world)
{
    if ((ent == world) || (ent == NULL)) {
        return;
    }
    Eris::Entity * cont = ent->getContainer();
    moveTo(cont, world);
    WFMath::Point<3> pos = ent->getPosition();
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
    float winsize = std::max(view.get_width(), view.get_height());
    m_antTexture = view.getAnts();

    m_renderMode = view.getRenderMode(m_name);
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
        SigC::slot(*this, &ServerEntities::entityChanged),
        ent));
    ent->Moved.connect(SigC::slot(*this, &ServerEntities::entityMoved));
    m_model.update(); // cause a re-draw
}

void ServerEntities::entityChanged(const Eris::StringSet &attrs, Eris::Entity *ent)
{
    m_model.update();   // a bit excessive I suppose
}

void ServerEntities::entityMoved(const WFMath::Point<3> &)
{
    m_model.update();   // a bit excessive I suppose
}

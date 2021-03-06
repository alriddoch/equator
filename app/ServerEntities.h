// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef EQUATOR_APP_SERVERENTITIES_H
#define EQUATOR_APP_SERVERENTITIES_H

#include "Layer.h"
#include "GlView.h"

#include <Atlas/Message/Element.h>

#include <wfmath/vector.h>

#include <set>

class Server;
class EntityImportOptions;
class EntityExportOptions;

namespace WFMath {
  template<const int dim> class AxisBox;
  class Quaternion;
}

namespace Eris {
  class Entity;
  class TypeInfo;
  
  // typedef StringSet -- doesn't work
  typedef std::set<std::string> StringSet;
}

namespace Atlas {
  namespace Message {
    class Object;
    class Encoder;
  }
}

namespace Gtk {
  class FileSelection;
}

class ServerEntities : public Layer {
  public:
    typedef std::set<Eris::Entity*> entlist_t;
    typedef std::map<int,Eris::Entity*> entname_t;
    typedef std::vector<Eris::Entity*> entstack_t;
    typedef enum { SELECT_INVERT, SELECT_ALL } selectMod_t;

    Server & m_serverConnection;
  private:
    GlView::rmode_t m_renderMode;
    Eris::Entity * m_world;
    Eris::Entity * m_selectionPrimary;
    entlist_t m_selectionList;
    entstack_t m_selectionStack;
    entlist_t m_lowVertexSelection;
    entlist_t m_highVertexSelection;
    bool m_validDrag;
    WFMath::Vector<3> m_dragPoint;

    int m_nameCount;
    entname_t m_nameDict;

    static EntityImportOptions * m_importOptions;
    static EntityExportOptions * m_exportOptions;

    sigc::connection m_loadOptionsDone;
    sigc::connection m_saveOptionsDone;

    Eris::TypeInfo * m_gameEntityType;

    void draw3DCube(const WFMath::AxisBox<3> & bbox, bool open = false);
    void draw3DSelectedBox(GlView & view, const WFMath::AxisBox<3> & bbox);
    void drawSelectableCorners(GlView & view, Eris::Entity *);
    void draw3DBox(const WFMath::AxisBox<3> & bbox);
    void drawEntity(GlView &, Eris::Entity *, Eris::Entity *, entstack_t::const_iterator);
    void drawWorld(GlView &, Eris::Entity * wrld);
    void moveTo(Eris::Entity * ent, Eris::Entity * wrld);

    void selectCorners(GlView & view, Eris::Entity *,
                       entname_t &, entname_t &);
    void selectEntity(GlView &, Eris::Entity *, entname_t & nameDict,
                      entstack_t::const_iterator);
    bool selectEntities(GlView &, int, int, int, int, bool check = false);

    void selectEntityVertices(GlView &, Eris::Entity *,
                              entname_t & lowDict, entname_t & highDict,
                              entstack_t::const_iterator);
    bool selectVertices(GlView &, int, int, int, int, bool check = false);

    void newType(Eris::TypeInfo*);
    void descendTypeTree(Eris::TypeInfo*);
    void alignEntityParent(Eris::Entity * ent);
    void connectEntity(Eris::Entity * );

    void modifyEntitySelection(Eris::Entity *, entstack_t::const_iterator,
                               selectMod_t, const entlist_t &,
                               const entlist_t &, const entlist_t &);
    void modifySelection(selectMod_t);

    void loadOptions(Gtk::FileSelection *);
    void saveOptions(Gtk::FileSelection *);

    void insertEntityContents(const std::string & container,
                              const Atlas::Message::MapType & ent,
                              const Atlas::Message::MapType & entities);
    void exportEntity(const std::string & id,
                      Atlas::Message::Encoder & e, Eris::Entity *);

    void load(Gtk::FileSelection *);
    void save(Gtk::FileSelection *);

    void cancel(Gtk::FileSelection *);

    void createOptionsWindows();
    void orient(const WFMath::Quaternion & orientation);
  public:
    ServerEntities(Model &, Server &);

    virtual void options();
    virtual void importFile();
    virtual void exportFile();
    virtual void selectInvert();
    virtual void selectAll();
    virtual void selectNone();

    void draw(GlView & view);
    bool animate(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void pushSelection();
    void popSelection();
    void dragStart(GlView & view, int x, int y);
    void dragUpdate(GlView & view, const WFMath::Vector<3> &);
    void dragEnd(GlView & view, const WFMath::Vector<3> &);
    void insert(const PosType &);
    void align(Alignment );
    
    void gotNewEntity(Eris::Entity *);
    void entityChanged(const Eris::StringSet &attrs);
    void entityMoved();
};

#endif // EQUATOR_APP_SERVERENTITIES_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERENTITIES_H
#define EQUATOR_APP_SERVERENTITIES_H

#include "Layer.h"
#include "GlView.h"

#include <Atlas/Message/Element.h>

#include <set>

class Server;
class ImportOptions;
class ExportOptions;

namespace WFMath {
  template<const int dim> class AxisBox;
  template<const int dim> class Point;
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
  private:
    Server & m_serverConnection;
    GlView::rmode_t m_renderMode;
    Eris::Entity * m_selection; 
    entlist_t m_selectionList; 
    entstack_t m_selectionStack; 
    bool m_validDrag;

    int m_nameCount;
    entname_t m_nameDict;
    unsigned int m_antTexture;

    static ImportOptions * m_importOptions;
    static ExportOptions * m_exportOptions;

    SigC::Connection m_loadOptionsDone;
    SigC::Connection m_saveOptionsDone;

    Eris::TypeInfo * m_gameEntityType;

    void draw3DCube(const WFMath::AxisBox<3> & bbox, bool open = false);
    void draw3DSelectedBox(const WFMath::AxisBox<3> & bbox, float phase = 0);
    void draw3DBox(const WFMath::AxisBox<3> & bbox);
    void drawEntity(Eris::Entity *, Eris::Entity *, entstack_t::const_iterator);
    void drawWorld(Eris::Entity * wrld);
    void moveTo(Eris::Entity * ent, Eris::Entity * wrld);
    void selectEntity(Eris::Entity*,Eris::Entity *, entstack_t::const_iterator);
    bool selectEntities(GlView & view, int nx, int ny, int fx, int fy,
                        bool check = false);
    void newType(Eris::TypeInfo*);
    void descendTypeTree(Eris::TypeInfo*);
    void alignEntityParent(Eris::Entity * ent);
    void connectEntity(Eris::Entity * );

    void loadOptions(Gtk::FileSelection *);
    void saveOptions(Gtk::FileSelection *);

    void insertEntityContents(const std::string & container,
                              const Atlas::Message::Element::MapType & ent,
                              const Atlas::Message::Element::MapType & entities);
    void exportEntity(const std::string & id,
                      Atlas::Message::Encoder & e, Eris::Entity *);

    void load(Gtk::FileSelection *);
    void save(Gtk::FileSelection *);

    void cancel(Gtk::FileSelection *);

    void createOptionsWindows();
    void orient(const WFMath::Quaternion & orientation);
  public:
    ServerEntities(Model &, Server &);
    void importFile();
    void exportFile();
    void draw(GlView & view);
    void animate(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void pushSelection();
    void popSelection();
    void dragStart(GlView & view, int x, int y);
    void dragUpdate(GlView & view, float x, float y, float z);
    void dragEnd(GlView & view, float x, float y, float z);
    void insert(const WFMath::Point<3> &);
    void align(Alignment );
    
    void gotNewEntity(Eris::Entity *ent);
    void entityChanged(const Eris::StringSet &attrs, Eris::Entity *ent);
    void entityMoved(const WFMath::Point<3> &);
};

#endif // EQUATOR_APP_SERVERENTITIES_H

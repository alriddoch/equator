// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERENTITIES_H
#define EQUATOR_APP_SERVERENTITIES_H

#include "Layer.h"
#include "GlView.h"

#include <map>
#include <vector>

class Server;
class Vector3D;

namespace Eris {
  class BBox;
  class Entity;
  class TypeInfo;
}

class ServerEntities : public Layer {
  public:
    typedef std::map<Eris::Entity*,int> entlist_t;
    typedef std::map<int,Eris::Entity*> entname_t;
    typedef std::vector<Eris::Entity*> entstack_t;
  private:
    Server & m_serverConnection;
    GlView::rmode_t m_renderMode;
    Eris::Entity * m_selection; 
    entlist_t m_selectionList; 
    entstack_t m_selectionStack; 

    int m_nameCount;
    entname_t m_nameDict;

    Eris::TypeInfo * m_gameEntityType;

    void draw3DCube(const Vector3D & coords, const Eris::BBox & bbox,
                    bool open = false);
    void draw3DBox(const Vector3D & coords, const Eris::BBox & bbox);
    void drawEntity(Eris::Entity * ent, entstack_t::const_iterator);
    void drawWorld(Eris::Entity * wrld);
    void selectEntity(Eris::Entity * ent, entstack_t::const_iterator);
    bool selectSingleEntity(GlView & view, int nx, int ny, bool check = false);
    void newType(Eris::TypeInfo*);
    void descendTypeTree(Eris::TypeInfo*);
  public:
    ServerEntities(Model &, Server &);
    void importFile() { }
    void draw(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void pushSelection();
    void popSelection();
    void dragStart(GlView & view, int x, int y) { }
    void dragUpdate(GlView & view, float x, float y, float z) { }
    void dragEnd(GlView & view, float x, float y, float z) { }
    void insert(const Vector3D &);
};

#endif // EQUATOR_APP_SERVERENTITIES_H

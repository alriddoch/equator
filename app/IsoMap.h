// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_ISOMAP_H
#define EQUATOR_APP_ISOMAP_H

#include "Layer.h"

#include <set>

namespace Coal {
  class Container;
  class Component;
}

namespace Gtk {
  class FileSelection;
}

class TileMap;

class IsoMap : public Layer {
  private:
    Coal::Container & m_database;
    //std::map<CoalRegion *,int> m_selection;
    std::set<std::pair<int, int> > m_selection;
    bool m_validDrag;
    int m_antTexture;
    TileMap * m_tileMap;

    void buildTileMap(Coal::Container & map_base);

    // void animateMapRegion(CoalRegion & map_region, float);
    void animateMap(GlView & , float);

    // void drawMapRegion(CoalRegion & map_region);
    // void drawMapObject(CoalObject & map_object);
    void drawMap(GlView & view);

    bool selectMap(GlView & view, Coal::Container & map_base, int, int, int, int, bool check = false);

    void load(Gtk::FileSelection *);
    void save(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);
    void installTiles();
  public:
    explicit IsoMap(Model &);
    void importFile();
    void exportFile();
    void draw(GlView & view);
    void animate(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void pushSelection() { }
    void popSelection() { }
    void dragStart(GlView & view, int x, int y);
    void dragUpdate(GlView & view, float x, float y, float z);
    void dragEnd(GlView & view, float x, float y, float z);
    void insert(const WFMath::Vector<3> &) { }
    void align(Alignment ) { }
};

#endif // EQUATOR_APP_ISOMAP_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_BLADEMAP_H
#define EQUATOR_APP_BLADEMAP_H

#include "Layer.h"

#include <map>

namespace Coal {
  class Landscape;
  class Component;
  class Container;
}

namespace Gtk {
  class FileSelection;
}

class BladeMap : public Layer {
  private:
    Coal::Container & m_database;
    std::map<Coal::Component *,int> m_selection;
    bool m_validDrag;

    void drawMapRegion(Coal::Landscape & map_region);
    void drawMap(Coal::Container & map_base);
    bool selectMap(GlView &, Coal::Container & map_base,int,int,int,int, bool check = false);
    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);
    void installTextures();
  public:
    explicit BladeMap(Model &);
    void importFile();
    void exportFile() { }
    void draw(GlView & view);
    void animate(GlView & view) { }
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

#endif // EQUATOR_APP_BLADEMAP_H

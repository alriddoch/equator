// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_BLADEMAP_H
#define EQUATOR_APP_BLADEMAP_H

#include "Layer.h"

#include <map>

class CoalRegion;
class CoalObject;
class CoalDatabase;

namespace Gtk {
  class FileSelection;
}

class BladeMap : public Layer {
  private:
    CoalDatabase & m_database;
    std::map<CoalRegion *,int> m_selection;
    bool m_validDrag;

    void drawMapRegion(CoalRegion & map_region);
    void drawMapObject(CoalObject & map_object);
    void drawMap(CoalDatabase & map_base);
    bool selectMap(GlView &, CoalDatabase & map_base,int,int,int,int, bool check = false);
    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);
  public:
    explicit BladeMap(Model &);
    void importFile();
    void draw(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void dragStart(GlView & view, int x, int y);
    void dragUpdate(GlView & view, float x, float y, float z);
    void dragEnd(GlView & view, float x, float y, float z);
};

#endif // EQUATOR_APP_BLADEMAP_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_ISOMAP_H
#define EQUATOR_APP_ISOMAP_H

#include "Layer.h"

#include <map>

class CoalRegion;
class CoalObject;
class CoalDatabase;

namespace Gtk {
  class FileSelection;
}

class IsoMap : public Layer {
  private:
    CoalDatabase & m_database;
    std::map<CoalRegion *,int> m_selection;
    bool m_validDrag;

    void drawMapRegion(CoalRegion & map_region);
    void drawMapObject(CoalObject & map_object);
    void drawMap(CoalDatabase & map_base);

    bool selectMap(CoalDatabase & map_base, int, int, int, int, bool check = false);

    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);
  public:
    explicit IsoMap(GlView &);
    void importFile();
    void draw();
    void select(int x, int y);
    void select(int x, int y, int w, int h);
    void dragStart(int x, int y);
    void dragUpdate(float x, float y, float z);
    void dragEnd(float x, float y, float z);
};

#endif // EQUATOR_APP_ISOMAP_H

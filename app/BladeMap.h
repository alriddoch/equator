// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_BLADEMAP_H
#define EQUATOR_APP_BLADEMAP_H

#include "Layer.h"

class CoalRegion;
class CoalObject;
class CoalDatabase;

namespace Gtk {
  class FileSelection;
}

class BladeMap : public Layer {
  private:
    CoalDatabase & m_database;

    void drawMapRegion(CoalRegion & map_region);
    void drawMapObject(CoalObject & map_object);
    void drawMap(CoalDatabase & map_base);
    void load(Gtk::FileSelection *);
  public:
    explicit BladeMap(const GlView &);
    void draw();
};

#endif // EQUATOR_APP_BLADEMAP_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_HEIGHTMANAGER_H
#define EQUATOR_HEIGHTMANAGER_H

#include "Layer.h"

#include <set>

namespace Gtk {
  class FileSelection;
}

class GlView;
class HeightMap;

class HeightManager : public Layer {
  private:
    std::set<HeightMap *> m_selection;

    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);

    void selectRegion(HeightMap * map);
    void outlineRegion(HeightMap * map, float);
    void heightMapRegion(HeightMap * map);
    void drawRegion(GlView & view, HeightMap * map);
  public:
    explicit HeightManager(Model &);

    void importFile();
    void exportFile() { }
    void draw(GlView & view);
    void animate(GlView & view);
    void select(GlView & view, int x, int y);
    void select(GlView & view, int x, int y, int w, int h);
    void pushSelection() { }
    void popSelection() { }
    void dragStart(GlView & view, int x, int y) { }
    void dragUpdate(GlView & view, float x, float y, float z) { }
    void dragEnd(GlView & view, float x, float y, float z) { }
    void insert(const WFMath::Point<3> &) { }
};

#endif // EQUATOR_HEIGHTMANAGER_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_ENTITYLAYER_H
#define EQUATOR_APP_ENTITYLAYER_H

#include "Layer.h"

namespace Gtk {
  class FileSelection;
}

// template <class ET>
class EntityLayer : public Layer {
  private:
    void importFile() { }
    void exportFile() { }
    void load(Gtk::FileSelection *) { }
    void cancel(Gtk::FileSelection *) { }
  public:
    explicit EntityLayer(Model &);
    void draw(GlView & view);
    void select(GlView & view, int x, int y) { }
    void select(GlView & view, int x, int y, int w, int h) { }
    void pushSelection() { }
    void popSelection() { }
    void dragStart(GlView & view, int x, int y) { }
    void dragUpdate(GlView & view, float x, float y, float z) { }
    void dragEnd(GlView & view, float x, float y, float z) { }
    void insert(const PosType &) { }
    void align(Alignment ) { }
};

#endif // EQUATOR_APP_ENTITYLAYER_H

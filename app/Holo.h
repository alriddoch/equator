// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_HOLO_H
#define EQUATOR_APP_HOLO_H

#include "Layer.h"

namespace Gtk {
  class FileSelection;
}

class Holo : public Layer {
  private:
    void importFile() { }
    void load(Gtk::FileSelection *) { }
    void cancel(Gtk::FileSelection *) { }
  public:
    explicit Holo(Model &);
    void draw(GlView & view);
    void select(GlView & view, int x, int y) { }
    void select(GlView & view, int x, int y, int w, int h) { }
    void dragStart(GlView & view, int x, int y) { }
    void dragUpdate(GlView & view, float x, float y, float z) { }
    void dragEnd(GlView & view, float x, float y, float z) { }
};

#endif // EQUATOR_APP_HOLO_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef EQUATOR_APP_HOLO_H
#define EQUATOR_APP_HOLO_H

#include "Layer.h"

namespace Gtk {
  class FileSelection;
}

class Holo : public Layer {
  private:
    void load(Gtk::FileSelection *) { }
    void cancel(Gtk::FileSelection *) { }
  public:
    explicit Holo(Model &);

    virtual void options() { }
    virtual void importFile() { }
    virtual void exportFile() { }
    virtual void selectInvert() { }
    virtual void selectAll() { }
    virtual void selectNone() { }

    virtual void draw(GlView & view);
    virtual bool animate(GlView & view);
    virtual void select(GlView & view, int x, int y) { }
    virtual void select(GlView & view, int x, int y, int w, int h) { }
    virtual void pushSelection() { }
    virtual void popSelection() { }
    virtual void dragStart(GlView & view, int x, int y) { }
    virtual void dragUpdate(GlView & view, float x, float y, float z) { }
    virtual void dragEnd(GlView & view, float x, float y, float z) { }
    virtual void insert(const PosType &) { }
    virtual void align(Alignment ) { }
};

#endif // EQUATOR_APP_HOLO_H

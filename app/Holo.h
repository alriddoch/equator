// Equator Online RPG World Building Tool
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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

    int m_box_radius;
  public:
    explicit Holo(Model &);

    void setBoxSize(int r);

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
    virtual void dragUpdate(GlView & view, const WFMath::Vector<3> &) { }
    virtual void dragEnd(GlView & view, const WFMath::Vector<3> &) { }
    virtual void insert(const PosType &) { }
    virtual void align(Alignment ) { }
};

#endif // EQUATOR_APP_HOLO_H

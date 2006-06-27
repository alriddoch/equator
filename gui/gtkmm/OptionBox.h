// Equator Online RPG World Building Tool
// Copyright (C) 2004 Alistair Riddoch
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

#ifndef EQUATOR_GUI_GTKMM_OPTION_BOX_H
#define EQUATOR_GUI_GTKMM_OPTION_BOX_H

#include <gtkmm/box.h>

// Base class from which all option windows inherit. In fact all option
// windows are just boxes which are used in a common dock class.
// It has no public constructor.

class OptionBox : public Gtk::VBox {
  private:
    Gtk::Window * m_dock;
  protected:
    OptionBox(const Glib::ustring & title);

    std::list<Gtk::TargetEntry> m_targetList;

  public:
    void setDock(Gtk::Window * dock);

    Gtk::Window * getDock() const {
        return m_dock;
    }
};

#endif // EQUATOR_GUI_GTKMM_OPTION_BOX_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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

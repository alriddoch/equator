// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_DOCK_WINDOW_H
#define EQUATOR_GUI_GTKMM_DOCK_WINDOW_H

#include <gtkmm/window.h>

class OptionBox;

namespace Gtk {
  class VBox;
} // Gtk

// Dock window for taking OptionBox objects.

class DockWindow : public Gtk::Window {
  protected:
    Gtk::VBox * m_vbox;
  public:
    DockWindow(OptionBox &);
};

#endif // EQUATOR_GUI_GTKMM_DOCK_WINDOW_H

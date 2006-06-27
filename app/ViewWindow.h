// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

#ifndef EQUATOR_APP_VIEWWINDOW_H
#define EQUATOR_APP_VIEWWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class GlView;
class Model;

namespace Gtk {
  class Statusbar;
  class VRuler;
  class HRuler;
  class VScrollbar;
  class HScrollbar;
};

class ViewWindow : public Gtk::Window
{
  private:
    GlView * m_glarea;
    Gtk::Statusbar * m_cursorCoords;
    int m_cursorContext;
    Gtk::Statusbar * m_viewCoords;
    int m_viewContext;
    Gtk::VRuler * m_vRuler;
    Gtk::HRuler * m_hRuler;
    Gtk::VScrollbar * m_vScrollbar;
    Gtk::HScrollbar * m_hScrollbar;
    Gtk::Adjustment * m_vAdjust;
    Gtk::Adjustment * m_hAdjust;
    float m_dAdjust;
    bool m_scrollLock;

  public:
    ViewWindow(MainWindow &, Model &);

    bool deleteEvent(GdkEventAny*) {
        // Fix it here so it don't close
        return 1;
    }

    GlView * getView() const {
        return m_glarea;
    }

    void setTitle();
    void cursorMoved();
    void vAdjustChanged();
    void hAdjustChanged();
    void glViewChanged();

    void updateViewCoords();
    void updateRulers();

    void motionNotifyEvent(GdkEventMotion * event);
};

#endif // EQUATOR_APP_VIEWWINDOW_H

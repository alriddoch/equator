// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_VIEWWINDOW_H
#define EQUATOR_APP_VIEWWINDOW_H

#include <gtkgl--/glarea.h>
#include <gtk--/window.h>
#include <gtk--/menu.h>

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
    Gtk::VRuler * m_vRuler;
    Gtk::HRuler * m_hRuler;
    Gtk::VScrollbar * m_vScrollbar;
    Gtk::HScrollbar * m_hScrollbar;
    Gtk::Adjustment * m_north;
    Gtk::Adjustment * m_east;
    int m_cursorContext;

  public:
    MainWindow & m_mainWindow;

    ViewWindow(MainWindow &, Model &);

    gint delete_event_impl(GdkEventAny*) {
        // Fix it here so it don't close
        return 1;
    }

    GlView * getView() const {
        return m_glarea;
    }

    void setTitle();
    void cursorMoved();
    void northChanged();
    void eastChanged();
};

#endif // EQUATOR_APP_VIEWWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_INHERITANCEWINDOW_H
#define EQUATOR_APP_INHERITANCEWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class AtlasMapWidget;

namespace Gtk {
  class Tree;
};

class InheritanceWindow : public Gtk::Window
{
  private:
    Gtk::Label * m_connectionLabel;
    Gtk::Tree * m_classTree;
    AtlasMapWidget * m_attributeTree;

  public:
    MainWindow & m_mainWindow;

    explicit InheritanceWindow(MainWindow &);

    gint delete_event_impl(GdkEventAny*) {
        hide();
        return 1;
    }

    gint buttonEvent(GdkEventButton*);

};

#endif // EQUATOR_APP_INHERITANCEWINDOW_H

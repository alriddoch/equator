// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERWINDOW_H
#define EQUATOR_APP_SERVERWINDOW_H

#include <gtk--/window.h>
#include <gtk--/clist.h>

class MainWindow;

namespace Gtk {
  class Tree;
  class CTree;
};

class ServerWindow : public Gtk::Window
{
  private:
    Gtk::Label * m_serverLabel;

  public:
    MainWindow & m_mainWindow;

    explicit ServerWindow(MainWindow &);

    gint buttonEvent(GdkEventButton*);

};

#endif // EQUATOR_APP_SERVERWINDOW_H

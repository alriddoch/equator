// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_NEWSERVERWINDOW_H
#define EQUATOR_APP_NEWSERVERWINDOW_H

#include <gtk--/window.h>
#include <gtk--/list.h>

class MainWindow;

class NewServerWindow : public Gtk::Window
{
  private:
    MainWindow & m_mainWindow;
  public:
    NewServerWindow(MainWindow & m);

    void doshow();

    void okay();
    void cancel();
};

#endif // EQUATOR_APP_NEWSERVERWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERWINDOW_H
#define EQUATOR_APP_SERVERWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class Server;
class NewServerWindow;

namespace Gtk {
  class OptionMenu;
};

class ServerWindow : public Gtk::Window
{
  private:
    Gtk::OptionMenu * m_serverMenu;
    Server * m_currentServer;

  public:
    MainWindow & m_mainWindow;

    explicit ServerWindow(MainWindow &);

    gint delete_event_impl(GdkEventAny*) {
        hide();
        return 1;
    }

    void newServer(Server *);
    void setServer(Server *);
};

#endif // EQUATOR_APP_SERVERWINDOW_H

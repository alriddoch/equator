// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERWINDOW_H
#define EQUATOR_APP_SERVERWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class Server;
class ConnectWindow;
class LoginWindow;
class CharacterWindow;

namespace Gtk {
  class OptionMenu;
};

class ServerWindow : public Gtk::Window
{
  private:
    Gtk::OptionMenu * m_serverMenu;
    Server * m_currentServer;

    ConnectWindow & m_connectWindow;
    LoginWindow * m_loginWindow;
    CharacterWindow * m_characterWindow;

  public:
    MainWindow & m_mainWindow;

    explicit ServerWindow(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void connect();

    void newServer(Server *);
    void setServer(Server *);
};

#endif // EQUATOR_APP_SERVERWINDOW_H

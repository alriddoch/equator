// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LOGIN_WINDOW_H
#define EQUATOR_APP_LOGIN_WINDOW_H

#include <gtkmm/dialog.h>

class Server;

namespace Gtk {
  class Entry;
  class OptionMenu;
  class SpinButton;
  class Button;
  class Statusbar;
}

namespace Atlas {
  namespace Objects {
    namespace Entity {
      class Player;
    }
  }
}

class LoginWindow : public Gtk::Dialog
{
  private:
    Gtk::Entry * m_userEntry;
    Gtk::Entry * m_passwdEntry;
    Gtk::Button * m_loginButton;

    Gtk::Statusbar * m_status;
    int m_statusContext;
    Server * m_server;

    SigC::Connection m_failure;
    SigC::Connection m_loggedIn;
  public:
    LoginWindow();

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();

    void useServer(Server *);

    void login();
    void create();

    void failure(const std::string & msg);
    void loggedIn(const Atlas::Objects::Entity::Player & );

    void dismiss(int);

    SigC::Signal1<void, Server *> loginSuccess;
};

#endif // EQUATOR_APP_LOGIN_WINDOW_H

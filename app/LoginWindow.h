// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LOGIN_WINDOW_H
#define EQUATOR_APP_LOGIN_WINDOW_H

#include <gtkmm/dialog.h>

class Server;

namespace Gtk {
  class Entry;
  class Combo;
  class Button;
  class Statusbar;
  class Label;
}

class LoginWindow : public Gtk::Dialog
{
  private:
    Gtk::Label * m_serverLabel;
    Gtk::Combo * m_userEntry;
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
    void loggedIn();

    void dismiss(int);

    SigC::Signal1<void, Server *> loginSuccess;
};

#endif // EQUATOR_APP_LOGIN_WINDOW_H

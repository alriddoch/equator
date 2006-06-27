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

#ifndef EQUATOR_APP_LOGIN_WINDOW_H
#define EQUATOR_APP_LOGIN_WINDOW_H

#include <gtkmm/dialog.h>

class Server;

namespace Gtk {
  class Label;
  class Combo;
  class Entry;
  class Button;
  class Statusbar;
}

class LoginWindow : public Gtk::Dialog
{
  private:
    Gtk::Label * m_serverLabel;
    Gtk::Combo * m_userEntry;
    Gtk::Entry * m_passwdEntry;
    Gtk::Button * m_createButton;
    Gtk::Button * m_loginButton;
    Gtk::Statusbar * m_status;

    int m_statusContext;
    Server * m_server;

    sigc::connection m_failure;
    sigc::connection m_loggedIn;
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

    void response(int);

    sigc::signal<void, Server *> loginSuccess;
};

#endif // EQUATOR_APP_LOGIN_WINDOW_H

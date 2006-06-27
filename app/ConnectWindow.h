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

#ifndef EQUATOR_APP_CONNECT_WINDOW_H
#define EQUATOR_APP_CONNECT_WINDOW_H

#include <gtkmm/dialog.h>

class Server;
class MainWindow;

namespace Gtk {
  class Entry;
  class OptionMenu;
  class SpinButton;
  class Button;
  class Statusbar;
}

class ConnectWindow : public Gtk::Dialog
{
  private:
    Gtk::Entry * m_hostEntry;
    Gtk::OptionMenu * m_portChoice;
    Gtk::SpinButton * m_portSpin;
    Gtk::Button * m_connectButton;
    Gtk::Statusbar * m_status;

    int m_customPort;
    int m_portNum;
    int m_statusContext;
    Server * m_server;

    sigc::connection m_failure;
    sigc::connection m_connected;

    void setPort(int );
    void setCustomPort();
    void response(int);
  public:
    MainWindow & m_mainWindow;

    ConnectWindow(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();

    void createConnection();

    void failure(const std::string & msg);
    void connected();

    sigc::signal<void, Server *> serverConnected;
};

#endif // EQUATOR_APP_CONNECT_WINDOW_H

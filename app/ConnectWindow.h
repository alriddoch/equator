// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_NEWSERVERWINDOW_H
#define EQUATOR_APP_NEWSERVERWINDOW_H

#include <gtkmm/dialog.h>

class MainWindow;
class Server;
class Model;

namespace Eris {
  class Entity;
}

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

class ConnectWindow : public Gtk::Dialog
{
  private:
    MainWindow & m_mainWindow;

    Gtk::Entry * m_hostEntry;
    Gtk::OptionMenu * m_portChoice;
    Gtk::SpinButton * m_portSpin;
    Gtk::Button * m_connectButton;

    Gtk::Statusbar * m_status;
    int m_customPort;
    int m_portNum;
    int m_statusContext;
    Server * m_server;

    SigC::Connection m_failure;
    SigC::Connection m_connected;

    void setPort(int );
    void setCustomPort();
  public:
    ConnectWindow(MainWindow & m);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();

    void createConnection();

    void failure(const std::string & msg);
    void connected();

    void dismiss(int);
};

#endif // EQUATOR_APP_NEWSERVERWINDOW_H

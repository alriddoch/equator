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

class NewServerWindow : public Gtk::Dialog
{
  private:
    MainWindow & m_mainWindow;

    Gtk::Entry * m_hostEntry;
    Gtk::OptionMenu * m_portChoice;
    Gtk::OptionMenu * m_characterChoice;
    Gtk::SpinButton * m_portSpin;
    Gtk::Button * m_connectButton;
    Gtk::Entry * m_userEntry;
    Gtk::Entry * m_passwdEntry;
    Gtk::Entry * m_avatarNameEntry;
    Gtk::Entry * m_avatarTypeEntry;
    Gtk::Button * m_loginButton;
    Gtk::Button * m_createButton;
    Gtk::Button * m_avatarButton;
    Gtk::Button * m_viewButton;
    Gtk::Statusbar * m_status;
    int m_customPort;
    int m_portNum;
    int m_statusContext;
    Server * m_server;

    std::string m_selectedCharacterId;

    SigC::Connection m_failure;
    SigC::Connection m_connected;
    SigC::Connection m_loggedIn;
    SigC::Connection m_worldEnter;

    void setSelectedCharacter(std::string s);
    void setPort(int );
    void setCustomPort();
  public:
    NewServerWindow(MainWindow & m);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();

    void createConnection();
    void loginAccount();
    void createAccount();
    void takeAvatar();
    void createAvatar();
    void createView();

    void failure(const std::string & msg);
    void connected();
    void loginComplete(const Atlas::Objects::Entity::Player &);
    void worldEnter(Eris::Entity*);

    void dismiss(int);
};

#endif // EQUATOR_APP_NEWSERVERWINDOW_H

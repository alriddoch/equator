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
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
};

class ServerWindow : public Gtk::Window
{
  private:
    Glib::RefPtr<Gtk::ListStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_hostnameColumn;
    Gtk::TreeModelColumn<Server *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

    Server * m_currentServer;

    ConnectWindow & m_connectWindow;
    LoginWindow & m_loginWindow;
    CharacterWindow & m_characterWindow;

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
    void loggedIn(Server *);
    void createdAvatar(Server *);
};

#endif // EQUATOR_APP_SERVERWINDOW_H

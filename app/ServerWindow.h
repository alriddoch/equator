// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_SERVERWINDOW_H
#define EQUATOR_APP_SERVERWINDOW_H

#include "gui/gtkmm/OptionBox.h"

class MainWindow;
class Server;
class ConnectWindow;
class LoginWindow;
class CharacterWindow;
class TypeTree;

namespace Gtk {
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
   class Menu;
};

class ServerWindow : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::ListStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_hostnameColumn;
    Gtk::TreeModelColumn<Server *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Gtk::Menu * m_popupMenu;

    Server * m_currentServer;
    std::map<Server *, TypeTree *> m_typeTrees;

    ConnectWindow & m_connectWindow;
    LoginWindow & m_loginWindow;
    CharacterWindow & m_characterWindow;

    bool buttonPressEvent(GdkEventButton*);
    void newServer(Server *);
    void loggedIn(Server *);
    void typesPressed();

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

  public:
    MainWindow & m_mainWindow;

    explicit ServerWindow(MainWindow &);

    void connect();

    void setServer(Server *);
};

#endif // EQUATOR_APP_SERVERWINDOW_H

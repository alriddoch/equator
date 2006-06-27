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

#ifndef EQUATOR_APP_SERVERWINDOW_H
#define EQUATOR_APP_SERVERWINDOW_H

#include "gui/gtkmm/OptionBox.h"

class MainWindow;
class Server;
class ConnectWindow;
class LoginWindow;
class CharacterWindow;
class ChatWindow;
class TypeTree;

namespace Gtk {
   class Action;
   class ActionGroup;
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
   class UIManager;
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
    Glib::RefPtr< Gtk::UIManager > m_UIManager;
    Glib::RefPtr< Gtk::ActionGroup > m_actions;
    Glib::RefPtr< Gtk::Action > m_actionConnect;
    Glib::RefPtr< Gtk::Action > m_actionLogin;
    Glib::RefPtr< Gtk::Action > m_actionCharacter;
    Glib::RefPtr< Gtk::Action > m_actionChat;
    Glib::RefPtr< Gtk::Action > m_actionStatus;
    Glib::RefPtr< Gtk::Action > m_actionTypes;
    Glib::RefPtr< Gtk::Action > m_actionDisconnect;

    Server * m_currentServer;
    std::map<Server *, TypeTree *> m_typeTrees;
    std::map< Server *, ChatWindow * > m_chatWindows;

    ConnectWindow & m_connectWindow;
    LoginWindow & m_loginWindow;
    CharacterWindow & m_characterWindow;

    void buttonPressEvent(GdkEventButton*);
    void newServer(Server *);
    void loggedIn(Server *);
    void connectPressed();
    void loginPressed();
    void characterPressed();
    void chatPressed();
    void statusPressed();
    void typesPressed();
    void disconnectPressed();
    void selectionChanged();

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

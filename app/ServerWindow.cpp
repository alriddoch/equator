// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerWindow.h"

#include "MainWindow.h"
#include "ConnectWindow.h"
#include "LoginWindow.h"
#include "CharacterWindow.h"
#include "Model.h"
#include "Server.h"

#include "gui/gtkmm/DockWindow.h"
#include "gui/gtkmm/TypeTree.h"

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/uimanager.h>

#include <sigc++/object_slot.h>

#include <iostream>
#include <vector>

#include <cassert>

#include <Eris/Connection.h>

static const Glib::ustring g_sUI = ""
"<ui>"
"  <popup name='PopupMenu'>"
"    <menuitem action='Connect'/>"
"    <separator/>"
"    <menuitem action='Login'/>"
"    <menuitem action='Character'/>"
"    <separator/>"
"    <menuitem action='Status'/>"
"    <menuitem action='Types' />"
"    <separator/>"
"    <menuitem action='Disconnect'/>"
"  </popup>"
"  <toolbar name='ToolBar'>"
"    <toolitem action='Connect'/>"
"    <toolitem action='Types'/>"
"  </toolbar>"
"</ui>";

ServerWindow::ServerWindow(MainWindow & mw) : OptionBox("Servers"),
                                      m_connectWindow(*new ConnectWindow(mw)),
                                      m_loginWindow(*new LoginWindow()),
                                      m_characterWindow(*new CharacterWindow()),
                                      m_mainWindow(mw)
{
    Gtk::VBox * vbox = this;

    m_columns = new Gtk::TreeModelColumnRecord();
    m_hostnameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_ptrColumn = new Gtk::TreeModelColumn<Server*>();
    m_columns->add(*m_hostnameColumn);
    m_columns->add(*m_ptrColumn);

    m_treeModel = Gtk::ListStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("Hostname", *m_hostnameColumn);
    m_treeView->signal_button_press_event().connect_notify(SigC::slot(*this, &ServerWindow::buttonPressEvent));

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    m_refTreeSelection->signal_changed().connect(sigc::mem_fun(this, &ServerWindow::selectionChanged));

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);
    m_actionConnect = Gtk::Action::create("Connect", "Connect ...", "Connect to a server.");
    m_actionLogin = Gtk::Action::create("Login", "Login ...", "Log inton the server.");
    m_actionCharacter = Gtk::Action::create("Character", "Character ...", "Take or create a character.");
    m_actionStatus = Gtk::Action::create("Status", "Status", "TODO: Tooltip.");
    m_actionTypes = Gtk::Action::create("Types", "Types", "TODO: Tooltip");
    m_actionDisconnect = Gtk::Action::create("Disconnect", "Disconnect", "Disconnect from the server.");
    m_actions = Gtk::ActionGroup::create();
    m_actions->add(m_actionConnect, sigc::mem_fun(this, &ServerWindow::connectPressed));
    m_actions->add(m_actionLogin, sigc::mem_fun(this, &ServerWindow::loginPressed));
    m_actions->add(m_actionCharacter, sigc::mem_fun(this, &ServerWindow::characterPressed));
    m_actions->add(m_actionStatus, sigc::mem_fun(this, &ServerWindow::statusPressed));
    m_actions->add(m_actionTypes, sigc::mem_fun(this, &ServerWindow::typesPressed));
    m_actions->add(m_actionDisconnect, sigc::mem_fun(this, &ServerWindow::disconnectPressed));
    m_UIManager = Gtk::UIManager::create();
    m_UIManager->insert_action_group(m_actions);
    m_UIManager->add_ui_from_string(g_sUI);
    m_popupMenu = dynamic_cast< Gtk::Menu * >(m_UIManager->get_widget("/PopupMenu"));
    vbox->pack_start(*(m_UIManager->get_widget("/ToolBar")), Gtk::PACK_SHRINK, 6);
    signal_delete_event().connect(SigC::slot(*this, &ServerWindow::deleteEvent));
    m_connectWindow.serverConnected.connect(SigC::slot(*this, &ServerWindow::newServer));
    m_loginWindow.loginSuccess.connect(SigC::slot(*this, &ServerWindow::loggedIn));
    // call this so the correct buttons get disabled
    selectionChanged();
}

void ServerWindow::buttonPressEvent(GdkEventButton * event)
{
    std::cout << "Button press event" << std::endl << std::flush;
    if (event->button == 3) {
        // FIXME: this is called so that changes to the state are propagated to the UI
        //        could this better be done via eager evaluation?
        selectionChanged();
        m_popupMenu->popup(event->button, event->time);
    }
    // return TRUE;
}

void ServerWindow::connect()
{
    m_connectWindow.show_all();
}

void ServerWindow::newServer(Server * server)
{
    assert(server != 0);

    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_hostnameColumn] = Glib::ustring(server->getName());
    row[*m_ptrColumn] = server;

    m_refTreeSelection->select(row);

    m_loginWindow.useServer(server);
    m_loginWindow.show_all();
}

void ServerWindow::setServer(Server * server)
{
    m_currentServer = server;

    // FIXME Update the contents of the window to reflect the newly selected
    // server

}

void ServerWindow::loggedIn(Server * server)
{
    assert(server != 0);

    m_characterWindow.useServer(server);
    m_characterWindow.show_all();
}

void ServerWindow::connectPressed()
{
    // TODO: fill the window with the server information of selected server
    m_connectWindow.doshow();
}

void ServerWindow::loginPressed()
{
    // TODO: fill the window with correct account according to selected server
    m_loginWindow.doshow();
}

void ServerWindow::characterPressed()
{
    // TODO: fill the window with correct characters of selected server and account
    m_characterWindow.doshow();
}

void ServerWindow::statusPressed()
{
    Gtk::TreeModel::Row row = *(m_refTreeSelection->get_selected());
    Server * selServer = row[*m_ptrColumn];

    assert(selServer != 0);

    // FIXME Query status
}

void ServerWindow::typesPressed()
{
    Gtk::TreeModel::Row row = *(m_refTreeSelection->get_selected());
    Server * selServer = row[*m_ptrColumn];

    assert(selServer != 0);

    std::map<Server *, TypeTree *>::const_iterator I = m_typeTrees.find(selServer);

    if (I == m_typeTrees.end()) {
        TypeTree * tw = new TypeTree(*selServer);
        (new DockWindow(*tw))->show_all();
        m_typeTrees[selServer] = tw;
        tw->populate();
    } else {
        I->second->getDock()->show_all();
    }
}

void ServerWindow::disconnectPressed()
{
    Gtk::TreeIter Iterator(m_refTreeSelection->get_selected());
    
    if(Iterator == true) {
        Server * pServer((*Iterator)[*m_ptrColumn]);
        
        assert(pServer != 0);
        // TODO: should this be done from here?
        if(pServer->isConnected() == true) {
            pServer->m_connection->disconnect();
        }
    }
}

void ServerWindow::selectionChanged(void)
{
    Gtk::TreeIter Iterator(m_refTreeSelection->get_selected());
    
    if(Iterator == true) {
        Server * pServer((*Iterator)[*m_ptrColumn]);
        
        assert(pServer != 0);
        m_actionLogin->set_sensitive((pServer->isConnected() == true) && (pServer->isLoggedIn() == false));
        m_actionCharacter->set_sensitive((pServer->isLoggedIn() == true) && (pServer->isInGame() == false));
        m_actionStatus->set_sensitive(pServer->isConnected() == true);
        m_actionTypes->set_sensitive(pServer->isConnected() == true);
        m_actionDisconnect->set_sensitive(pServer->isConnected() == true);
    }
    else {
        m_actionConnect->set_sensitive(true);
        m_actionLogin->set_sensitive(false);
        m_actionCharacter->set_sensitive(false);
        m_actionStatus->set_sensitive(false);
        m_actionTypes->set_sensitive(false);
        m_actionDisconnect->set_sensitive(false);
    }
}

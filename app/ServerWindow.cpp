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

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>

#include <sigc++/object_slot.h>

#include <iostream>
#include <vector>

#include <cassert>

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
    // m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &ServerWindow::selectionChanged) );

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);

    Gtk::HBox * bothbox = manage( new Gtk::HBox );

    Gtk::Button * b = manage( new Gtk::Button("Types...") );
    b->signal_clicked().connect(SigC::slot(*this, &ServerWindow::typesPressed));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 6);

    m_popupMenu = manage( new Gtk::Menu );
    Gtk::Menu_Helpers::MenuList & server_menu = m_popupMenu->items();
    server_menu.push_back(Gtk::Menu_Helpers::MenuElem("Login..."));
    server_menu.push_back(Gtk::Menu_Helpers::MenuElem("Character..."));
    server_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    server_menu.push_back(Gtk::Menu_Helpers::MenuElem("Types...", SigC::slot(*this, &ServerWindow::typesPressed)));
    server_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    server_menu.push_back(Gtk::Menu_Helpers::MenuElem("Disconnect..."));

    signal_delete_event().connect(SigC::slot(*this, &ServerWindow::deleteEvent));
    m_connectWindow.serverConnected.connect(SigC::slot(*this, &ServerWindow::newServer));
    m_loginWindow.loginSuccess.connect(SigC::slot(*this, &ServerWindow::loggedIn));
}

void ServerWindow::buttonPressEvent(GdkEventButton * event)
{
    std::cout << "Button press event" << std::endl << std::flush;
    if (event->button == 3) {
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

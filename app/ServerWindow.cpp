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

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>

#include <iostream>
#include <vector>

#include <cassert>

ServerWindow::ServerWindow(MainWindow & mw) :
                                      m_connectWindow(*new ConnectWindow()),
                                      m_loginWindow(*new LoginWindow()),
                                      m_characterWindow(*new CharacterWindow()),
                                      m_mainWindow(mw)
{
    // destroy.connect(slot(this, &ServerWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    m_columns = new Gtk::TreeModelColumnRecord();
    m_hostnameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_ptrColumn = new Gtk::TreeModelColumn<Server*>();
    m_columns->add(*m_hostnameColumn);
    m_columns->add(*m_ptrColumn);

    m_treeModel = Gtk::ListStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("Hostname", *m_hostnameColumn);

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    // m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &ServerWindow::selectionChanged) );

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);

    add(*vbox);
    set_title("Servers");

    // show_all();
    signal_delete_event().connect(slot(*this, &ServerWindow::deleteEvent));
    m_connectWindow.serverConnected.connect(slot(*this, &ServerWindow::newServer));
    m_loginWindow.loginSuccess.connect(slot(*this, &ServerWindow::loggedIn));
    m_characterWindow.createSuccess.connect(slot(*this, &ServerWindow::createdAvatar));
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

void ServerWindow::createdAvatar(Server * server)
{
    assert(server != 0);

    Model & model = m_mainWindow.newModel();
    model.setName(server->getName());
    server->createLayers(model);
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "LoginWindow.h"

#include "MainWindow.h"
#include "Server.h"
#include "Model.h"

#include <Eris/Connection.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Player.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/entry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/main.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include <cassert>

LoginWindow::LoginWindow() :
                 Gtk::Dialog("Connect to server", false, true),
                 m_userEntry(0), m_passwdEntry(0),
                 m_loginButton(0), m_status(0),
                 m_server(0)
{
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    signal_response().connect(slot(*this, &LoginWindow::dismiss));

    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox(false, 6) );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage(new Gtk::Label("Login:"))));
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );

    Gtk::Table * table = manage( new Gtk::Table(2, 3) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Username:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_userEntry = manage( new Gtk::Entry() );
    m_userEntry->set_max_length(60);
    table->attach(*m_userEntry, 1, 3, 0, 1);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Password:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_passwdEntry = manage( new Gtk::Entry() );
    m_passwdEntry->set_max_length(60);
    table->attach(*m_passwdEntry, 1, 3, 1, 2);

    hbox->pack_start(*table);

    vbox->pack_start(*hbox);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    m_loginButton = manage( new Gtk::Button("_Login", true) );
    m_loginButton->signal_clicked().connect(slot(*this, &LoginWindow::login));
    a->add(*m_loginButton);
    vbox->pack_start(*a);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Login status");
    // vbox->pack_start(*m_status, Gtk::AttachOptions(0), 0);

    signal_delete_event().connect(slot(*this, &LoginWindow::deleteEvent));
}

void LoginWindow::doshow()
{
    m_userEntry->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_loginButton->set_sensitive(true);
    show_all();
}

void LoginWindow::useServer(Server * s)
{
    assert(s != 0);

    m_server = s;
}

void LoginWindow::login()
{
    assert(m_server != 0);

    m_status->push("Logging in...", m_statusContext);

    m_userEntry->set_editable(false);
    m_passwdEntry->set_editable(false);
    m_loginButton->set_sensitive(false);

    // m_failure = m_server->m_connection.Failure.connect(SigC::slot(*this, &LoginWindow::failure));
    m_loggedIn = m_server->m_connection.getLobby()->LoggedIn.connect(SigC::slot(*this, &LoginWindow::loggedIn));

    //std::cout << m_hostEntry->get_text() << ": " << m_portNum
              //<< std::endl << std::flush;
    // c.connect(m_hostEntry->get_text(), m_portNum);

    //Gtk::Main::input.connect(slot(server, &Server::poll), c.getSocket(),
                             //GDK_INPUT_READ);
    // m_server->connect("localhost", 6767);
    m_server->login(m_userEntry->get_text(), m_passwdEntry->get_text());
}

void LoginWindow::failure(const std::string & msg)
{
    assert(m_server != 0);

    std::cout << "LoginWindow::failure" << std::endl << std::flush;
    std::cout << msg << std::endl << std::flush;

    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    m_failure.disconnect();
    m_loggedIn.disconnect();

    m_userEntry->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_loginButton->set_sensitive(true);
}

void LoginWindow::loggedIn(const Atlas::Objects::Entity::Player &)
{
    assert(m_server != 0);

    m_status->pop(m_statusContext);
    m_status->push("Logged In", m_statusContext);

    loginSuccess.emit(m_server);

    m_server = 0;

    m_userEntry->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_loginButton->set_sensitive(true);

    std::cout << "Got connection success in LoginWindow" << std::endl
              << std::flush;

    hide();
}

void LoginWindow::dismiss(int response)
{
    m_failure.disconnect();
    m_loggedIn.disconnect();
    hide();
}
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

#include "LoginWindow.h"

#include "Server.h"

#include <Eris/Account.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/combo.h>
#include <gtkmm/entry.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/messagedialog.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/hide.h>

#include <iostream>
#include <cassert>

LoginWindow::LoginWindow() :
                 Gtk::Dialog("Login to server", false, true),
                 m_userEntry(0), m_passwdEntry(0),
                 m_createButton(0), m_loginButton(0), m_status(0),
                 m_server(0)
{
    m_createButton = add_button("_Create Account", Gtk::RESPONSE_YES);
    m_createButton->set_use_underline();
    m_loginButton = add_button("_Login", Gtk::RESPONSE_ACCEPT);
    m_loginButton->set_use_underline();
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    set_default_response(Gtk::RESPONSE_ACCEPT);
    signal_response().connect(sigc::mem_fun(*this, &LoginWindow::response));

    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox(false, 6) );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    m_serverLabel = manage( new Gtk::Label("Login to XXX:"));
    a->add(*m_serverLabel);
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );

    Gtk::Table * table = manage( new Gtk::Table(2, 3) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Username:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_userEntry = manage( new Gtk::Combo() );
    m_userEntry->get_entry()->set_max_length(60);
    m_userEntry->get_entry()->set_activates_default();
    table->attach(*m_userEntry, 1, 3, 0, 1);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Password:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_passwdEntry = manage( new Gtk::Entry() );
    m_passwdEntry->set_visibility(false);
    m_passwdEntry->set_max_length(60);
    m_passwdEntry->set_activates_default();
    table->attach(*m_passwdEntry, 1, 3, 1, 2);

    hbox->pack_start(*table);

    vbox->pack_start(*hbox);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Login status");
    // vbox->pack_start(*m_status, Gtk::PACK_SHRINK, 0);

    signal_delete_event().connect(sigc::mem_fun(*this, &LoginWindow::deleteEvent));
}

void LoginWindow::doshow()
{
    m_userEntry->get_entry()->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_createButton->set_sensitive(true);
    m_loginButton->set_sensitive(true);
    show_all();
}

void LoginWindow::useServer(Server * s)
{
    assert(s != 0);

    m_server = s;
    m_serverLabel->set_text(std::string("Login to ") + m_server->getName() + ":");
}

void LoginWindow::create()
{
    assert(m_server != 0);

    m_status->push("Creating account...", m_statusContext);

    m_userEntry->get_entry()->set_editable(false);
    m_passwdEntry->set_editable(false);
    m_createButton->set_sensitive(false);
    m_loginButton->set_sensitive(false);

    m_server->createAccount(m_userEntry->get_entry()->get_text(), m_passwdEntry->get_text());

    m_failure = m_server->m_account->LoginFailure.connect(sigc::mem_fun(*this, &LoginWindow::failure));
    m_loggedIn = m_server->m_account->LoginSuccess.connect(sigc::mem_fun(*this, &LoginWindow::loggedIn));
}

void LoginWindow::login()
{
    assert(m_server != 0);

    m_status->push("Logging in...", m_statusContext);

    m_userEntry->get_entry()->set_editable(false);
    m_passwdEntry->set_editable(false);
    m_createButton->set_sensitive(false);
    m_loginButton->set_sensitive(false);

    m_server->login(m_userEntry->get_entry()->get_text(), m_passwdEntry->get_text());

    m_failure = m_server->m_account->LoginFailure.connect(sigc::mem_fun(*this, &LoginWindow::failure));
    m_loggedIn = m_server->m_account->LoginSuccess.connect(sigc::mem_fun(*this, &LoginWindow::loggedIn));
}

void LoginWindow::failure(const std::string & msg)
{
    assert(m_server != 0);

    Gtk::MessageDialog * md = new Gtk::MessageDialog(*this, "Login Failed", false, Gtk::MESSAGE_ERROR);
    md->signal_response().connect(sigc::hide(sigc::mem_fun(*md, &Gtk::MessageDialog::hide)));
    md->show();

    std::cout << "LoginWindow::failure" << std::endl << std::flush;
    std::cout << msg << std::endl << std::flush;

    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    m_failure.disconnect();
    m_loggedIn.disconnect();

    m_userEntry->get_entry()->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_createButton->set_sensitive(true);
    m_loginButton->set_sensitive(true);
}

void LoginWindow::loggedIn()
{
    assert(m_server != 0);

    m_status->pop(m_statusContext);
    m_status->push("Logged In", m_statusContext);

    loginSuccess.emit(m_server);

    m_server = 0;

    m_userEntry->get_entry()->set_editable(true);
    m_passwdEntry->set_editable(true);
    m_loginButton->set_sensitive(true);

    std::cout << "Got connection success in LoginWindow" << std::endl
              << std::flush;

    hide();
}

void LoginWindow::response(int response)
{
    if (response == Gtk::RESPONSE_ACCEPT) {
        login();
    } else if (response == Gtk::RESPONSE_YES) {
        create();
    } else if (response == Gtk::RESPONSE_CLOSE) {
        m_failure.disconnect();
        m_loggedIn.disconnect();
        hide();
    }
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "CharacterWindow.h"

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

CharacterWindow::CharacterWindow() :
                 Gtk::Dialog("Connect to server", false, true),
                 m_nameEntry(0), m_typeEntry(0),
                 m_createButton(0), m_status(0),
                 m_server(0)
{
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    signal_response().connect(slot(*this, &CharacterWindow::dismiss));

    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox(false, 6) );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage(new Gtk::Label("Create Character:"))));
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );

    Gtk::Table * table = manage( new Gtk::Table(2, 3) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Name:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_nameEntry = manage( new Gtk::Entry() );
    m_nameEntry->set_max_length(60);
    table->attach(*m_nameEntry, 1, 3, 0, 1);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Type:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_typeEntry = manage( new Gtk::Entry() );
    m_typeEntry->set_max_length(60);
    table->attach(*m_typeEntry, 1, 3, 1, 2);

    hbox->pack_start(*table);

    vbox->pack_start(*hbox);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    m_createButton = manage( new Gtk::Button("_Create", true) );
    m_createButton->signal_clicked().connect(slot(*this, &CharacterWindow::create));
    a->add(*m_createButton);
    vbox->pack_start(*a);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Login status");
    // vbox->pack_start(*m_status, Gtk::AttachOptions(0), 0);

    signal_delete_event().connect(slot(*this, &CharacterWindow::deleteEvent));
}

void CharacterWindow::doshow()
{
    m_nameEntry->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);
    show_all();
}

void CharacterWindow::useServer(Server * s)
{
    assert(s != 0);

    m_server = s;
}

void CharacterWindow::create()
{
    assert(m_server != 0);

    m_status->push("Logging in...", m_statusContext);

    m_nameEntry->set_editable(false);
    m_typeEntry->set_editable(false);
    m_createButton->set_sensitive(false);

    // m_failure = m_server->m_connection.Failure.connect(SigC::slot(*this, &CharacterWindow::failure));

    //std::cout << m_hostEntry->get_text() << ": " << m_portNum
              //<< std::endl << std::flush;
    // c.connect(m_hostEntry->get_text(), m_portNum);

    //Gtk::Main::input.connect(slot(server, &Server::poll), c.getSocket(),
                             //GDK_INPUT_READ);
    // m_server->connect("localhost", 6767);
    m_server->createCharacter(m_nameEntry->get_text(), m_typeEntry->get_text());
    m_created = m_server->m_world->Entered.connect(SigC::slot(*this, &CharacterWindow::created));
}

void CharacterWindow::failure(const std::string & msg)
{
    assert(m_server != 0);

    std::cout << "CharacterWindow::failure" << std::endl << std::flush;
    std::cout << msg << std::endl << std::flush;

    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    m_failure.disconnect();
    m_created.disconnect();

    m_nameEntry->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);
}

void CharacterWindow::created(Eris::Entity *)
{
    assert(m_server != 0);

    m_status->pop(m_statusContext);
    m_status->push("Logged In", m_statusContext);

    createSuccess.emit(m_server);

    m_server = 0;

    m_nameEntry->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);

    std::cout << "Got connection success in CharacterWindow" << std::endl
              << std::flush;

    hide();
}

void CharacterWindow::dismiss(int response)
{
    m_failure.disconnect();
    m_created.disconnect();
    hide();
}

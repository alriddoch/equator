// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ConnectWindow.h"

#include "MainWindow.h"
#include "Server.h"

#include <Eris/Connection.h>

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

ConnectWindow::ConnectWindow(MainWindow & mw) :
                 Gtk::Dialog("Connect to server", false, true),
                 m_hostEntry(0), m_portChoice(0), m_portSpin(0),
                 m_connectButton(0), m_status(0),
                 m_customPort(6767), m_portNum(6767), m_server(0),
                 m_mainWindow(mw)
{
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    signal_response().connect(slot(*this, &ConnectWindow::dismiss));

    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox(false, 6) );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage(new Gtk::Label("Server:"))));
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );

    Gtk::Table * table = manage( new Gtk::Table(2, 3) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Hostname:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_hostEntry = manage( new Gtk::Entry() );
    m_hostEntry->set_text("localhost");
    m_hostEntry->set_max_length(60);
    table->attach(*m_hostEntry, 1, 3, 0, 1);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Port:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_portChoice = manage( new Gtk::OptionMenu() );
    Gtk::Menu * portMenu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& portEntries = portMenu->items();
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<int>(slot(*this, &ConnectWindow::setPort), 6767)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<int>(slot(*this, &ConnectWindow::setPort), 6768)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", slot(*this, &ConnectWindow::setCustomPort)));
    m_portChoice->set_menu(*portMenu);
    table->attach(*m_portChoice, 1, 2, 1, 2);
    Gtk::Adjustment * adj = manage( new Gtk::Adjustment (6767.0, 1.0, 32768.0) );
    m_portSpin = manage( new Gtk::SpinButton(*adj, 1) );
    m_portSpin->set_sensitive(false);
    table->attach(*m_portSpin, 2, 3, 1, 2);
    hbox->pack_start(*table);

    vbox->pack_start(*hbox);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    m_connectButton = manage( new Gtk::Button("Co_nnect", true) );
    m_connectButton->signal_clicked().connect(slot(*this, &ConnectWindow::createConnection));
    a->add(*m_connectButton);
    vbox->pack_start(*a);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Connection status");
    // vbox->pack_start(*m_status, Gtk::PACK_SHRINK, 0);

    signal_delete_event().connect(slot(*this, &ConnectWindow::deleteEvent));
}

void ConnectWindow::setPort(int port)
{
    m_portNum = port;
    m_portSpin->get_adjustment()->set_value(port);
    m_portSpin->set_sensitive(false);
}

void ConnectWindow::setCustomPort()
{
    m_portNum = m_customPort;
    m_portSpin->set_sensitive(true);
}

void ConnectWindow::doshow()
{
    m_hostEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);
    show_all();
}

void ConnectWindow::createConnection()
{
    assert(m_server == 0);

    m_status->push("Connecting...", m_statusContext);

    m_server = &m_mainWindow.newServer(m_hostEntry->get_text());

    m_hostEntry->set_editable(false);
    m_connectButton->set_sensitive(false);
    m_portChoice->set_sensitive(false);

    m_failure = m_server->m_connection.Failure.connect(SigC::slot(*this, &ConnectWindow::failure));
    m_connected = m_server->m_connection.Connected.connect(SigC::slot(*this, &ConnectWindow::connected));

    //std::cout << m_hostEntry->get_text() << ": " << m_portNum
              //<< std::endl << std::flush;
    // c.connect(m_hostEntry->get_text(), m_portNum);

    //Gtk::Main::input.connect(slot(server, &Server::poll), c.getSocket(),
                             //GDK_INPUT_READ);
    // m_server->connect("localhost", 6767);
    m_server->connect(m_hostEntry->get_text(), m_portNum);
}

void ConnectWindow::failure(const std::string & msg)
{
    assert(m_server != 0);

    std::cout << "ConnectWindow::failure" << std::endl << std::flush;
    std::cout << msg << std::endl << std::flush;

    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    m_failure.disconnect();
    m_connected.disconnect();

    delete m_server;
    m_server = 0;

    m_hostEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);
}

void ConnectWindow::connected()
{
    assert(m_server != 0);

    m_status->pop(m_statusContext);
    m_status->push("Connected", m_statusContext);

    serverConnected.emit(m_server);

    m_server = 0;

    m_hostEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);

    std::cout << "Got connection success in ConnectWindow" << std::endl
              << std::flush;

    hide();
}

void ConnectWindow::dismiss(int response)
{
    m_failure.disconnect();
    m_connected.disconnect();
    hide();
}

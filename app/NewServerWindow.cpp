// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewServerWindow.h"
#include "MainWindow.h"
#include "Server.h"
#include "Model.h"
#include "ServerEntities.h"

#include <Eris/Connection.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>

#include <gtk--/box.h>
#include <gtk--/button.h>
#include <gtk--/label.h>
#include <gtk--/optionmenu.h>
#include <gtk--/entry.h>
#include <gtk--/spinbutton.h>
#include <gtk--/adjustment.h>
#include <gtk--/main.h>
#include <gtk--/statusbar.h>

NewServerWindow::NewServerWindow(MainWindow &m) :
                 Gtk::Window(GTK_WINDOW_TOPLEVEL), m_mainWindow(m),
                 m_hostEntry(NULL), m_portChoice(NULL),
                 m_portSpin(NULL), m_connectButton(NULL),
                 m_customPort(6767), m_portNum(6767),
                 m_server(NULL)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Hostname:") )), false, false, 2);
    m_hostEntry = manage( new Gtk::Entry(60) );
    hbox->pack_end(*m_hostEntry, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Port:") )), false, false, 2);
    m_portChoice = manage( new Gtk::OptionMenu() );
    Gtk::Menu * portMenu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& portEntries = portMenu->items();
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<int>(slot(this, &NewServerWindow::setPort), 6767)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<int>(slot(this, &NewServerWindow::setPort), 6768)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", slot(this, &NewServerWindow::setCustomPort)));
    m_portChoice->set_menu(portMenu);
    hbox->pack_start(*m_portChoice, false, false, 2);
    Gtk::Adjustment * adj = manage( new Gtk::Adjustment (6767.0, 1.0, 32768.0) );
    m_portSpin = manage( new Gtk::SpinButton(*adj, 1) );
    m_portSpin->set_sensitive(false);
    hbox->pack_end(*m_portSpin, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    m_connectButton = manage( new Gtk::Button("Connect") );
    m_connectButton->clicked.connect(slot(this, &NewServerWindow::createConnection));
    vbox->pack_start(*m_connectButton, false, false, 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Username:") )), false, false, 2);
    m_userEntry = manage( new Gtk::Entry(60) );
    hbox->pack_end(*m_userEntry, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Password:") )), false, false, 2);
    m_passwdEntry = manage( new Gtk::Entry(60) );
    m_passwdEntry->set_visibility(false);
    hbox->pack_end(*m_passwdEntry, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    hbox = manage( new Gtk::HBox() );
    m_loginButton = manage( new Gtk::Button("Login") );
    m_loginButton->clicked.connect(slot(this, &NewServerWindow::loginAccount));
    m_loginButton->set_sensitive(false);
    hbox->pack_start(*m_loginButton, true, true, 0);
    m_createButton = manage( new Gtk::Button("Create") );
    m_createButton->clicked.connect(slot(this, &NewServerWindow::createAccount));
    m_createButton->set_sensitive(false);
    hbox->pack_start(*m_createButton, true, true, 0);
    vbox->pack_start(*hbox, false, false, 0);

    vbox->pack_start(*(manage( new Gtk::Label("In Game Agent") )), false, false, 2);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Agent Name:") )), false, false, 2);
    m_agentNameEntry = manage( new Gtk::Entry(60) );
    hbox->pack_end(*m_agentNameEntry, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Agent Type:") )), false, false, 2);
    m_agentTypeEntry = manage( new Gtk::Entry(60) );
    hbox->pack_end(*m_agentTypeEntry, false, false, 2);
    vbox->pack_start(*hbox, false, false, 0);

    m_agentButton = manage( new Gtk::Button("Create Agent") );
    m_agentButton->clicked.connect(slot(this, &NewServerWindow::createAgent));
    m_agentButton->set_sensitive(false);
    vbox->pack_start(*m_agentButton, false, false, 0);

    m_viewButton = manage( new Gtk::Button("View") );
    m_viewButton->clicked.connect(slot(this, &NewServerWindow::createView));
    m_viewButton->set_sensitive(false);
    vbox->pack_start(*m_viewButton, false, false, 0);

    Gtk::Button * b = manage( new Gtk::Button("Dismiss") );
    b->clicked.connect(slot(this, &NewServerWindow::dismiss));
    vbox->pack_start(*b, false, false, 0);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Connection status");
    vbox->pack_start(*m_status, false, false, 0);

    add(*vbox);

    set_title("Connect to server");
}

void NewServerWindow::setPort(int port)
{
    m_portNum = port;
    m_portSpin->set_sensitive(false);
}

void NewServerWindow::setCustomPort()
{
    m_portNum = m_customPort;
    m_portSpin->set_sensitive(true);
}

void NewServerWindow::doshow()
{
    m_hostEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);
    show_all();
}

void NewServerWindow::createConnection()
{
    assert(m_server == NULL);

    m_status->push(m_statusContext, "Connecting...");

    m_server = new Server();

    m_failure = m_server->connection.Failure.connect(SigC::slot(this, &NewServerWindow::failure));
    m_connected = m_server->connection.Connected.connect(SigC::slot(this, &NewServerWindow::connected));

    //std::cout << m_hostEntry->get_text() << ": " << m_portNum
              //<< std::endl << std::flush;
    // c.connect(m_hostEntry->get_text(), m_portNum);

    //Gtk::Main::input.connect(slot(server, &Server::poll), c.getSocket(),
                             //GDK_INPUT_READ);
    m_server->connect("localhost", 6767);
    //server->connect(m_hostEntry->get_text(), m_portNum);
}

void NewServerWindow::loginAccount()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Logging in");

    m_server->login(m_userEntry->get_text(), m_passwdEntry->get_text());
    m_loggedIn = Eris::Lobby::instance()->LoggedIn.connect(SigC::slot(this, &NewServerWindow::loginComplete));
}

void NewServerWindow::createAccount()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Creating account");

    m_server->createAccount(m_userEntry->get_text(), m_passwdEntry->get_text());
    m_loggedIn = Eris::Lobby::instance()->LoggedIn.connect(SigC::slot(this, &NewServerWindow::loginComplete));
}

void NewServerWindow::createAgent()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Creating agent");

    m_server->createCharacter(m_agentNameEntry->get_text(), m_agentTypeEntry->get_text());

    m_worldEnter = m_server->world->Entered.connect(SigC::slot(this,&NewServerWindow::worldEnter));
}

void NewServerWindow::createView()
{
    SigC::Connection created = m_mainWindow.modelAdded.connect(slot(this, &NewServerWindow::viewCreated));
    m_mainWindow.newModel();
    created.disconnect();
    // FIXME Create us a view
}

void NewServerWindow::failure(const std::string & msg)
{
    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Connection failed");

    std::cout << "Got connection failure in NewServerWindow" << std::endl
              << std::flush;
    std::cout << msg << std::endl << std::flush;
}

void NewServerWindow::connected()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Connected");

    m_hostEntry->set_editable(false);
    m_connectButton->set_sensitive(false);
    m_portChoice->set_sensitive(false);
    m_loginButton->set_sensitive(true);
    m_createButton->set_sensitive(true);
    std::cout << "Got connection success in NewServerWindow" << std::endl
              << std::flush;
}

void NewServerWindow::loginComplete(const Atlas::Objects::Entity::Player &)
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Logged in");

    m_userEntry->set_editable(false);
    m_passwdEntry->set_text("");
    m_loginButton->set_sensitive(false);
    m_createButton->set_sensitive(false);
    m_agentButton->set_sensitive(true);
}

void NewServerWindow::worldEnter(Eris::Entity*)
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push(m_statusContext, "Agent created in world");

    m_agentNameEntry->set_editable(false);
    m_agentTypeEntry->set_editable(false);
    m_agentButton->set_sensitive(false);
    m_viewButton->set_sensitive(true);
}

void NewServerWindow::viewCreated(Model * model)
{
    model->setName(m_hostEntry->get_text());
    Layer * layer = new ServerEntities(*model, *m_server);
    model->addLayer(layer);
    m_viewButton->set_sensitive(false);
}

void NewServerWindow::dismiss()
{
    m_failure.disconnect();
    m_connected.disconnect();
    m_loggedIn.disconnect();
    m_worldEnter.disconnect();
    hide();
    // The rest of this code resets the status of the dialog sp that it can
    // be used to open a new connection. This cannot be done until Eris
    // adds support for multiple connections.
#if 0
    m_status->pop(m_statusContext);
    m_hostEntry->set_editable(true);
    m_userEntry->set_editable(true);
    m_agentNameEntry->set_editable(true);
    m_agentTypeEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);
    m_loginButton->set_sensitive(false);
    m_createButton->set_sensitive(false);
    m_viewButton->set_sensitive(false);
    m_server = false;
#endif
}

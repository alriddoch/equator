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

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/entry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/main.h>
#include <gtkmm/statusbar.h>

NewServerWindow::NewServerWindow(MainWindow & mw) :
                 Gtk::Window(Gtk::WINDOW_TOPLEVEL), m_mainWindow(mw),
                 m_hostEntry(NULL), m_portChoice(NULL),
                 m_portSpin(NULL), m_connectButton(NULL),
                 m_userEntry(NULL), m_passwdEntry(NULL),
                 m_avatarNameEntry(NULL), m_avatarTypeEntry(NULL),
                 m_loginButton(NULL), m_createButton(NULL),
                 m_avatarButton(NULL), m_viewButton(NULL),
                 m_status(NULL),
                 m_customPort(6767), m_portNum(6767),
                 m_server(NULL)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Hostname:") )), Gtk::AttachOptions(0), 2);
    m_hostEntry = manage( new Gtk::Entry() );
    m_hostEntry->set_text("localhost");
    m_hostEntry->set_max_length(60);
    hbox->pack_end(*m_hostEntry, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Port:") )), Gtk::AttachOptions(0), 2);
    m_portChoice = manage( new Gtk::OptionMenu() );
    Gtk::Menu * portMenu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& portEntries = portMenu->items();
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<int>(slot(*this, &NewServerWindow::setPort), 6767)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<int>(slot(*this, &NewServerWindow::setPort), 6768)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", slot(*this, &NewServerWindow::setCustomPort)));
    m_portChoice->set_menu(*portMenu);
    hbox->pack_start(*m_portChoice, Gtk::AttachOptions(0), 2);
    Gtk::Adjustment * adj = manage( new Gtk::Adjustment (6767.0, 1.0, 32768.0) );
    m_portSpin = manage( new Gtk::SpinButton(*adj, 1) );
    m_portSpin->set_sensitive(false);
    hbox->pack_end(*m_portSpin, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    m_connectButton = manage( new Gtk::Button("Connect") );
    m_connectButton->signal_clicked().connect(slot(*this, &NewServerWindow::createConnection));
    vbox->pack_start(*m_connectButton, Gtk::AttachOptions(0), 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Username:") )), Gtk::AttachOptions(0), 2);
    m_userEntry = manage( new Gtk::Entry() );
    m_userEntry->set_max_length(60);
    hbox->pack_end(*m_userEntry, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Password:") )), Gtk::AttachOptions(0), 2);
    m_passwdEntry = manage( new Gtk::Entry() );
    m_passwdEntry->set_visibility(false);
    m_passwdEntry->set_max_length(60);
    hbox->pack_end(*m_passwdEntry, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    hbox = manage( new Gtk::HBox() );
    m_loginButton = manage( new Gtk::Button("Login") );
    m_loginButton->signal_clicked().connect(slot(*this, &NewServerWindow::loginAccount));
    m_loginButton->set_sensitive(false);
    hbox->pack_start(*m_loginButton);
    m_createButton = manage( new Gtk::Button("Create") );
    m_createButton->signal_clicked().connect(slot(*this, &NewServerWindow::createAccount));
    m_createButton->set_sensitive(false);
    hbox->pack_start(*m_createButton);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    vbox->pack_start(*(manage( new Gtk::Label("In Game Avatar") )), Gtk::AttachOptions(0), 2);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Avatar Name:") )), Gtk::AttachOptions(0), 2);
    m_avatarNameEntry = manage( new Gtk::Entry() );
    m_avatarNameEntry->set_max_length(60);
    hbox->pack_end(*m_avatarNameEntry, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    hbox = manage( new Gtk::HBox() );
    hbox->pack_start(*(manage( new Gtk::Label("Avatar Type:") )), Gtk::AttachOptions(0), 2);
    m_avatarTypeEntry = manage( new Gtk::Entry() );
    m_avatarTypeEntry->set_max_length(60);
    hbox->pack_end(*m_avatarTypeEntry, Gtk::AttachOptions(0), 2);
    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    m_avatarButton = manage( new Gtk::Button("Create Avatar") );
    m_avatarButton->signal_clicked().connect(slot(*this, &NewServerWindow::createAvatar));
    m_avatarButton->set_sensitive(false);
    vbox->pack_start(*m_avatarButton, Gtk::AttachOptions(0), 0);

    m_viewButton = manage( new Gtk::Button("View") );
    m_viewButton->signal_clicked().connect(slot(*this, &NewServerWindow::createView));
    m_viewButton->set_sensitive(false);
    vbox->pack_start(*m_viewButton, Gtk::AttachOptions(0), 0);

    Gtk::Button * b = manage( new Gtk::Button("Dismiss") );
    b->signal_clicked().connect(slot(*this, &NewServerWindow::dismiss));
    vbox->pack_start(*b, Gtk::AttachOptions(0), 0);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Connection status");
    vbox->pack_start(*m_status, Gtk::AttachOptions(0), 0);

    add(*vbox);

    set_title("Connect to server");
    signal_delete_event().connect(slot(*this, &NewServerWindow::deleteEvent));
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

    m_status->push("Connecting...", m_statusContext);

    m_server = new Server();

    m_failure = m_server->connection.Failure.connect(SigC::slot(*this, &NewServerWindow::failure));
    m_connected = m_server->connection.Connected.connect(SigC::slot(*this, &NewServerWindow::connected));

    //std::cout << m_hostEntry->get_text() << ": " << m_portNum
              //<< std::endl << std::flush;
    // c.connect(m_hostEntry->get_text(), m_portNum);

    //Gtk::Main::input.connect(slot(server, &Server::poll), c.getSocket(),
                             //GDK_INPUT_READ);
    // m_server->connect("localhost", 6767);
    m_server->connect(m_hostEntry->get_text(), m_portNum);
}

void NewServerWindow::loginAccount()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Logging in", m_statusContext);

    m_server->login(m_userEntry->get_text(), m_passwdEntry->get_text());
    m_loggedIn = m_server->connection.getLobby()->LoggedIn.connect(SigC::slot(*this, &NewServerWindow::loginComplete));
}

void NewServerWindow::createAccount()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Creating account", m_statusContext);

    m_server->createAccount(m_userEntry->get_text(), m_passwdEntry->get_text());
    m_loggedIn = m_server->connection.getLobby()->LoggedIn.connect(SigC::slot(*this, &NewServerWindow::loginComplete));
}

void NewServerWindow::createAvatar()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Creating avatar", m_statusContext);

    m_server->createCharacter(m_avatarNameEntry->get_text(), m_avatarTypeEntry->get_text());

    m_worldEnter = m_server->world->Entered.connect(SigC::slot(*this,&NewServerWindow::worldEnter));
}

void NewServerWindow::createView()
{
    SigC::Connection created = m_mainWindow.modelAdded.connect(slot(*this, &NewServerWindow::viewCreated));
    m_mainWindow.newModel();
    created.disconnect();
    // FIXME Create us a view
}

void NewServerWindow::failure(const std::string & msg)
{
    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    std::cout << "Got connection failure in NewServerWindow" << std::endl
              << std::flush;
    std::cout << msg << std::endl << std::flush;
}

void NewServerWindow::connected()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Connected", m_statusContext);

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
    m_status->push("Logged in", m_statusContext);

    m_userEntry->set_editable(false);
    m_passwdEntry->set_text("");
    m_loginButton->set_sensitive(false);
    m_createButton->set_sensitive(false);
    m_avatarButton->set_sensitive(true);
}

void NewServerWindow::worldEnter(Eris::Entity*)
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Avatar created in world", m_statusContext);

    m_avatarNameEntry->set_editable(false);
    m_avatarTypeEntry->set_editable(false);
    m_avatarButton->set_sensitive(false);
    m_viewButton->set_sensitive(true);
}

void NewServerWindow::viewCreated(Model * model)
{
    model->setName(m_hostEntry->get_text());
    model->setServer(m_server);
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
#if 1
    m_status->pop(m_statusContext);
    m_hostEntry->set_editable(true);
    m_userEntry->set_editable(true);
    m_avatarNameEntry->set_editable(true);
    m_avatarTypeEntry->set_editable(true);
    m_connectButton->set_sensitive(true);
    m_portChoice->set_sensitive(true);
    m_loginButton->set_sensitive(false);
    m_createButton->set_sensitive(false);
    m_viewButton->set_sensitive(false);
    m_server = false;
#endif
}

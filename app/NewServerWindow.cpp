// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewServerWindow.h"

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

NewServerWindow::NewServerWindow(MainWindow & mw) :
                 Gtk::Dialog("Connect to server", false, true),
                 m_mainWindow(mw), m_hostEntry(0),
                 m_portChoice(0), m_portSpin(0),
                 m_connectButton(0), m_userEntry(0),
                 m_passwdEntry(0), m_avatarNameEntry(0),
                 m_avatarTypeEntry(0), m_loginButton(0),
                 m_createButton(0), m_avatarButton(0),
                 m_viewButton(0), m_status(0),
                 m_customPort(6767), m_portNum(6767), m_server(0)
{
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    signal_response().connect(slot(*this, &NewServerWindow::dismiss));

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
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<int>(slot(*this, &NewServerWindow::setPort), 6767)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<int>(slot(*this, &NewServerWindow::setPort), 6768)));
    portEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", slot(*this, &NewServerWindow::setCustomPort)));
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
    m_connectButton->signal_clicked().connect(slot(*this, &NewServerWindow::createConnection));
    a->add(*m_connectButton);
    vbox->pack_start(*a);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage(new Gtk::Label("Account:"))));
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );
    table = manage( new Gtk::Table(2, 2) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Username:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_userEntry = manage( new Gtk::Entry() );
    m_userEntry->set_max_length(60);
    table->attach(*m_userEntry, 1, 2, 0, 1);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Password:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_passwdEntry = manage( new Gtk::Entry() );
    m_passwdEntry->set_visibility(false);
    m_passwdEntry->set_max_length(60);
    table->attach(*m_passwdEntry, 1, 2, 1, 2);
    hbox->pack_start(*table);
    vbox->pack_start(*hbox);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    Gtk::HBox * v2box = manage( new Gtk::HBox(false, 6) );
    m_loginButton = manage( new Gtk::Button("_Login", true) );
    m_loginButton->signal_clicked().connect(slot(*this, &NewServerWindow::loginAccount));
    m_loginButton->set_sensitive(false);
    v2box->pack_start(*m_loginButton);
    m_createButton = manage( new Gtk::Button("Create _Account", true) );
    m_createButton->signal_clicked().connect(slot(*this, &NewServerWindow::createAccount));
    m_createButton->set_sensitive(false);
    v2box->pack_start(*m_createButton);
    a->add(*v2box);
    vbox->pack_start(*a);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage(new Gtk::Label("Avatar:"))));
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );
    table = manage( new Gtk::Table(3, 2) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Available Avatars:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_characterChoice = manage( new Gtk::OptionMenu() );
    Gtk::Menu * characterMenu = manage( new Gtk::Menu() );
    // Gtk::Menu_Helpers::MenuList& characterEntries = characterMenu->items();
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<int>(slot(*this, &NewServerWindow::setCharacter), 6767)));
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<int>(slot(*this, &NewServerWindow::setCharacter), 6768)));
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", slot(*this, &NewServerWindow::setCharacter)));
    m_characterChoice->set_menu(*characterMenu);
    table->attach(*m_characterChoice, 1, 2, 0, 1);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Avatar Name:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_avatarNameEntry = manage( new Gtk::Entry() );
    m_avatarNameEntry->set_max_length(60);
    table->attach(*m_avatarNameEntry, 1, 2, 1, 2);
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Avatar Type:") )));
    table->attach(*a, 0, 1, 2, 3, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_avatarTypeEntry = manage( new Gtk::Entry() );
    m_avatarTypeEntry->set_max_length(60);
    table->attach(*m_avatarTypeEntry, 1, 2, 2, 3);
    hbox->pack_start(*table);
    vbox->pack_start(*hbox);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    v2box = manage( new Gtk::HBox(false, 6) );
    m_takeAvatarButton = manage( new Gtk::Button("_Take Avatar", true) );
    m_takeAvatarButton->signal_clicked().connect(slot(*this, &NewServerWindow::takeAvatar));
    m_takeAvatarButton->set_sensitive(false);
    v2box->pack_start(*m_takeAvatarButton);
    m_avatarButton = manage( new Gtk::Button("C_reate Avatar", true) );
    m_avatarButton->signal_clicked().connect(slot(*this, &NewServerWindow::createAvatar));
    m_avatarButton->set_sensitive(false);
    v2box->pack_start(*m_avatarButton);
    m_viewButton = manage( new Gtk::Button("_View", true) );
    m_viewButton->signal_clicked().connect(slot(*this, &NewServerWindow::createView));
    m_viewButton->set_sensitive(false);
    v2box->pack_start(*m_viewButton);
    a->add(*v2box);
    vbox->pack_start(*a);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Connection status");
    // vbox->pack_start(*m_status, Gtk::AttachOptions(0), 0);

    signal_delete_event().connect(slot(*this, &NewServerWindow::deleteEvent));
}

void NewServerWindow::setSelectedCharacter(std::string charId)
{
    m_selectedCharacterId = charId;
    std::cout << charId << std::endl << std::flush;
    Eris::CharacterList chars = m_server->m_player->getCharacters();
    Eris::CharacterList::iterator I = chars.begin();
    std::cout << chars.size() << " characters" << std::endl << std::flush;
    for(; I != chars.end(); ++I) {
        if (charId == I->getId()) {
            std::cout << "GOT IT " << charId << " " << I->getId() << std::endl << std::flush;
            m_avatarNameEntry->set_text(I->getName());
            m_avatarTypeEntry->set_text(I->getParents().front().asString());
        }
    }

#warning FIXME Populate widgets with the information
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

    m_failure = m_server->m_connection.Failure.connect(SigC::slot(*this, &NewServerWindow::failure));
    m_connected = m_server->m_connection.Connected.connect(SigC::slot(*this, &NewServerWindow::connected));

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
    m_loggedIn = m_server->m_connection.getLobby()->LoggedIn.connect(SigC::slot(*this, &NewServerWindow::loginComplete));
}

void NewServerWindow::createAccount()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Creating account", m_statusContext);

    m_server->createAccount(m_userEntry->get_text(), m_passwdEntry->get_text());
    m_loggedIn = m_server->m_connection.getLobby()->LoggedIn.connect(SigC::slot(*this, &NewServerWindow::loginComplete));
}

void NewServerWindow::takeAvatar()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Creating avatar", m_statusContext);

    m_server->takeCharacter(m_selectedCharacterId);

    m_worldEnter = m_server->m_world->Entered.connect(SigC::slot(*this,&NewServerWindow::worldEnter));
}

void NewServerWindow::createAvatar()
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Creating avatar", m_statusContext);

    m_server->createCharacter(m_avatarNameEntry->get_text(), m_avatarTypeEntry->get_text());

    m_worldEnter = m_server->m_world->Entered.connect(SigC::slot(*this,&NewServerWindow::worldEnter));
}

void NewServerWindow::createView()
{
    Model & model = m_mainWindow.newModel();
    model.setName(m_hostEntry->get_text());
    // model->setServer(m_server);
    // Layer * layer = new HeightManager(*model);
    // model->addLayer(layer);
    // Layer * layer = new ServerEntities(model, *m_server);
    // model.addLayer(layer);
    m_server->createLayers(model);
    m_viewButton->set_sensitive(false);
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

    m_server->m_player->GotAllCharacters.connect(slot(*this, &NewServerWindow::gotCharacterList));
    m_server->m_player->refreshCharacterInfo();

    m_status->pop(m_statusContext);
    m_status->push("Logged in", m_statusContext);

    m_userEntry->set_editable(false);
    m_passwdEntry->set_text("");
    m_loginButton->set_sensitive(false);
    m_createButton->set_sensitive(false);
    m_avatarButton->set_sensitive(true);
    m_takeAvatarButton->set_sensitive(true);
}

void NewServerWindow::gotCharacterList()
{
    std::cout << "GOT CHARACTER LIST" << std::endl << std::flush;
    Gtk::Menu * characterMenu = m_characterChoice->get_menu();
    Gtk::Menu_Helpers::MenuList& characterEntries = characterMenu->items();
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Standard port", SigC::bind<std::string>(slot(*this, &NewServerWindow::setSelectedCharacter), "FOO")));
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Admin port", SigC::bind<std::string>(slot(*this, &NewServerWindow::setSelectedCharacter), "BAR")));
    // characterEntries.push_back(Gtk::Menu_Helpers::MenuElem("Custom port", SigC::bind<std::string>(slot(*this, &NewServerWindow::setSelectedCharacter), "BAZ")));
    Eris::CharacterList chars = m_server->m_player->getCharacters();
    Eris::CharacterList::iterator I = chars.begin();
    for(; I != chars.end(); ++I) {
        Atlas::Objects::Entity::GameEntity & ge = *I;
        characterEntries.push_back(Gtk::Menu_Helpers::MenuElem(ge.getId(), SigC::bind<std::string>(slot(*this, &NewServerWindow::setSelectedCharacter), ge.getId())));
        
    }
}

void NewServerWindow::worldEnter(Eris::Entity*)
{
    assert(m_server != NULL);

    m_status->pop(m_statusContext);
    m_status->push("Avatar created in world", m_statusContext);

    m_avatarNameEntry->set_editable(false);
    m_avatarTypeEntry->set_editable(false);
    m_avatarButton->set_sensitive(false);
    m_takeAvatarButton->set_sensitive(false);
    m_viewButton->set_sensitive(true);
}

void NewServerWindow::dismiss(int response)
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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerWindow.h"
#include "ViewWindow.h"
#include "GlView.h"
#include "Layer.h"

#include <gtkmm/frame.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>

#include <iostream>
#include <sstream>
#include <vector>

ServerWindow::ServerWindow(MainWindow & mw) : Gtk::Window(Gtk::WINDOW_TOPLEVEL),
                                              m_mainWindow(mw)
{
    // destroy.connect(slot(this, &ServerWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Server connection:") ) ), Gtk::AttachOptions(0), 2);
    m_serverMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_serverMenu, Gtk::EXPAND | Gtk::FILL, 2);
    tophbox->pack_end(*(manage( new Gtk::Label("WOOT") ) ), Gtk::AttachOptions(0), 2);
   
    vbox->pack_start(*tophbox, Gtk::AttachOptions(0), 2);

    add(*vbox);
    set_title("Servers");

    set_sensitive(false);

    // show_all();
    signal_delete_event().connect(slot(*this, &ServerWindow::deleteEvent));
}

void ServerWindow::newServer(Server * server)
{
    Gtk::Menu * menu = m_serverMenu->get_menu();
    std::stringstream ident;
    ident << /* server->getName() << */ "FIXME" /* << server->getModelNo() */;
    if (menu == NULL) {
        menu = manage( new Gtk::Menu() );
        
        Gtk::Menu_Helpers::MenuList& server_menu = menu->items();
        server_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Server*>(slot(*this, &ServerWindow::setServer),server)));
        m_serverMenu->set_menu(*menu);
        set_sensitive(true);
        setServer(server);
    } else {
        Gtk::Menu_Helpers::MenuList& server_menu = menu->items();
        server_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Server*>(slot(*this, &ServerWindow::setServer),server)));
    }
}

void ServerWindow::setServer(Server * server)
{
    m_currentServer = server;

    // FIXME Update the contents of the window to reflect the newly selected
    // server

}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerWindow.h"
#include "ViewWindow.h"
#include "GlView.h"
#include "Layer.h"

#include <gtk--/frame.h>
#include <gtk--/menuitem.h>
#include <gtk--/optionmenu.h>
#include <gtk--/box.h>
#include <gtk--/label.h>

#include <iostream>
#include <sstream>
#include <vector>

ServerWindow::ServerWindow(MainWindow & w) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                           m_mainWindow(w)
{
    // destroy.connect(slot(this, &ServerWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Server connection:") ) ), false, false, 2);
    m_serverMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_serverMenu, true, true, 2);
    tophbox->pack_end(*(manage( new Gtk::Label("WOOT") ) ), false, false, 2);
   
    vbox->pack_start(*tophbox, false, false, 2);

    add(*vbox);
    set_title("Servers");

    set_sensitive(false);

    // show_all();
}

void ServerWindow::newServer(Server * server)
{
    Gtk::Menu * menu = m_serverMenu->get_menu();
    std::stringstream ident;
    ident << /* server->getName() << */ "FIXME" /* << server->getModelNo() */;
    if (menu == NULL) {
        menu = manage( new Gtk::Menu() );
        
        Gtk::Menu_Helpers::MenuList& server_menu = menu->items();
        server_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Server*>(slot(this, &ServerWindow::setServer),server)));
        m_serverMenu->set_menu(menu);
        set_sensitive(true);
        setServer(server);
    } else {
        Gtk::Menu_Helpers::MenuList& server_menu = menu->items();
        server_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Server*>(slot(this, &ServerWindow::setServer),server)));
    }
}

void ServerWindow::setServer(Server * server)
{
    m_currentServer = server;

    // FIXME Update the contents of the window to reflect the newly selected
    // server

}

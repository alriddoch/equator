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

    // show_all();
}

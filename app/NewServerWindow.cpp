// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewServerWindow.h"
#include "MainWindow.h"

#include <gtk--/scrolledwindow.h>
#include <gtk--/box.h>
#include <gtk--/button.h>

NewServerWindow::NewServerWindow(MainWindow &m) :
                 Gtk::Window(GTK_WINDOW_TOPLEVEL), m_mainWindow(m)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    //vbox->pack_start(*scrolled_window, true, true, 2);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button("Okay") );
    b->clicked.connect(slot(this, &NewServerWindow::okay));
    hbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button("Cancel") );
    b->clicked.connect(slot(this, &NewServerWindow::cancel));
    hbox->pack_start(*b, true, true, 0);

    vbox->pack_start(*hbox, false, false, 0);

    add(*vbox);
}

void NewServerWindow::doshow()
{
    show_all();
}

void NewServerWindow::okay()
{
    hide();
}

void NewServerWindow::cancel()
{
    hide();
}

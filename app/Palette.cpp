// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Palette.h"

#include <gtk--/notebook.h>
#include <gtk--/box.h>

Palette::Palette(MainWindow & w) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                   m_mainWindow(w)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    m_notebook = manage( new Gtk::Notebook() );
    m_notebook->set_tab_pos(GTK_POS_TOP);

    vbox->pack_start(*m_notebook, true, true, 2);

    add(*vbox);
    set_title("Palette");
}

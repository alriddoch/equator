// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Palette.h"

#include <gtk--/notebook.h>
#include <gtk--/box.h>
#include <gtk--/clist.h>
#include <gtk--/scrolledwindow.h>

Palette::Palette(MainWindow & mw) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                    m_mainWindow(mw)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    m_notebook = manage( new Gtk::Notebook() );
    m_notebook->set_tab_pos(GTK_POS_TOP);

    //
    static const gchar *titles[] = { "Name", "Image", NULL };
    m_tile_clist = manage( new Gtk::CList(titles) );
    m_tile_clist->set_column_width(0, 200);
    m_tile_clist->set_column_width(1, 50);
    Gtk::ScrolledWindow * sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,150);
    sw->add(*m_tile_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Tiles"));

    static const gchar *entity_titles[] = { "Name", "Image", NULL };
    m_entity_clist = manage( new Gtk::CList(entity_titles) );
    m_entity_clist->set_column_width(0, 200);
    m_entity_clist->set_column_width(1, 50);
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,150);
    sw->add(*m_entity_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Entities"));

    static const gchar *texture_titles[] = { "Name", "Image", NULL };
    m_texture_clist = manage( new Gtk::CList(texture_titles) );
    m_texture_clist->set_column_width(0, 200);
    m_texture_clist->set_column_width(1, 50);
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,200);
    sw->add(*m_texture_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Textures"));

    vbox->pack_start(*m_notebook, true, true, 2);

    add(*vbox);
    set_title("Palette");
}

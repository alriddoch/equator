// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ViewWindow.h"
#include "GlView.h"

#include <gtk--/frame.h>
#include <gtk--/eventbox.h>
#include <gtk--/menuitem.h>

#include <iostream>

ViewWindow::ViewWindow(MainWindow & w) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                         m_glarea(NULL), m_popup(NULL),
                                         m_mainwindow(w)
{
    // destroy.connect(slot(this, &ViewWindow::destroy_handler));

    m_glarea = manage( new GlView(*this) );
    m_glarea->set_usize(300,300);

    Gtk::Frame * frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(GTK_SHADOW_IN);
    frame->set_border_width(4);
    frame->add(*m_glarea);

    add(*frame);

    m_popup = manage( new Gtk::Menu() );
    Gtk::MenuItem * menu_item = manage( new Gtk::MenuItem("Foo") );
    m_popup->append(*menu_item);
    menu_item = manage( new Gtk::MenuItem("Foo 2") );
    m_popup->append(*menu_item);
    menu_item = manage( new Gtk::MenuItem("Foo 3") );
    m_popup->append(*menu_item);

    show_all();
}

void ViewWindow::setTitle()
{
    if (m_glarea == NULL) {
        set_title(m_name);
    } else {
        set_title(m_name + m_glarea->details());
    }
}

void ViewWindow::setName(const std::string & n)
{
    m_name = n;
    setTitle();
}


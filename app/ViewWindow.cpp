// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ViewWindow.h"
#include "GlView.h"
#include "Model.h"

#include <gtk--/frame.h>
#include <gtk--/eventbox.h>
#include <gtk--/menuitem.h>

#include <iostream>

ViewWindow::ViewWindow(MainWindow & w, Model & m) :
                                         Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                         m_glarea(NULL), m_mainWindow(w)
{
    m.nameChanged.connect(slot(this, &ViewWindow::setTitle));
    // destroy.connect(slot(this, &ViewWindow::destroy_handler));

    m_glarea = manage( new GlView(w, *this, m) );
    m_glarea->set_usize(300,300);

    Gtk::Frame * frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(GTK_SHADOW_IN);
    frame->set_border_width(4);
    frame->add(*m_glarea);

    add(*frame);

    show_all();
}

void ViewWindow::setTitle()
{
    if (m_glarea == NULL) {
        set_title("");
    } else {
        set_title(m_glarea->m_model.getName() + m_glarea->details());
    }
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ViewWindow.h"
#include "GlView.h"
#include "Model.h"

#include <gtk--/frame.h>
#include <gtk--/eventbox.h>
#include <gtk--/menuitem.h>
#include <gtk--/box.h>
#include <gtk--/statusbar.h>
#include <gtk--/table.h>
#include <gtk--/ruler.h>
#include <gtk--/scrollbar.h>
#include <gtk--/adjustment.h>

#include <iostream>
#include <sstream>

ViewWindow::ViewWindow(MainWindow & w, Model & m) :
                                         Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                         m_glarea(NULL), m_mainWindow(w)
{
    m.nameChanged.connect(slot(this, &ViewWindow::setTitle));
    // destroy.connect(slot(this, &ViewWindow::destroy_handler));

    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    // This needs to be done before the GlView is created
    m_cursorCoords = manage( new Gtk::Statusbar() );
    m_cursorContext = m_cursorCoords->get_context_id("Cursor coordinates");
    m_cursorCoords->push(m_cursorContext, " xxx,xxx,xxx ");

    m_viewCoords = manage( new Gtk::Statusbar() );
    m_viewContext = m_viewCoords->get_context_id("View coordinates");
    m_viewCoords->push(m_cursorContext, " xxx,xxx,xxx ");

    m_glarea = manage( new GlView(w, *this, m) );
    m_glarea->set_usize(300,300);

    // Gtk::Frame * frame = manage( new Gtk::Frame() );
    // frame->set_shadow_type(GTK_SHADOW_IN);
    // frame->set_border_width(2);
    // frame->add(*m_glarea);

    // vbox->pack_start(*frame, true, true, 0);

    m_vAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_vAdjust->value_changed.connect(slot(this, &ViewWindow::vAdjustChanged));
    m_hAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_hAdjust->value_changed.connect(slot(this, &ViewWindow::hAdjustChanged));

    Gtk::Table * table = manage( new Gtk::Table(3, 3, false) );
    m_vRuler = manage( new Gtk::VRuler() );
    table->attach(*m_vRuler, 0, 1, 1, 2, GTK_FILL, GTK_FILL);
    m_hRuler = manage( new Gtk::HRuler() );
    table->attach(*m_hRuler, 1, 2, 0, 1, GTK_FILL, GTK_FILL);
    m_vScrollbar = manage( new Gtk::VScrollbar(*m_vAdjust) );
    table->attach(*m_vScrollbar, 2, 3, 1, 2, GTK_FILL, GTK_FILL);
    m_hScrollbar = manage( new Gtk::HScrollbar(*m_hAdjust) );
    table->attach(*m_hScrollbar, 1, 2, 2, 3, GTK_FILL, GTK_FILL);
    table->attach(*m_glarea, 1, 2, 1, 2);

    vbox->pack_start(*table, true, true, 0);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );

    hbox->pack_start(*m_cursorCoords, false, false, 2);
    hbox->pack_start(*m_viewCoords, false, false, 2);

    vbox->pack_start(*hbox, false, false, 0);

    add(*vbox);

    cursorMoved();
    viewMoved();

    show_all();

    float th = 0.0f, tv = 0.0f;

    m_glarea->getViewOffset(th, tv, m_dAdjust);
    std::cout << "Setting view things: " << th << ":" << tv << ":"
              << m_dAdjust << std::endl << std::flush;
    m_hAdjust->set_value(th);
    m_vAdjust->set_value(tv);
    m_glarea->viewChanged.connect(slot(this, &ViewWindow::glViewChanged));
}

void ViewWindow::setTitle()
{
    if (m_glarea == NULL) {
        set_title("");
    } else {
        set_title(m_glarea->m_model.getName() + m_glarea->details());
    }
}

void ViewWindow::cursorMoved()
{
    std::stringstream text;
    float x, y, z;
    m_glarea->m_model.getCursor(x, y, z);
    text << " " << x << "," << y << "," << z << " ";
    m_cursorCoords->pop(m_cursorContext);
    m_cursorCoords->push(m_cursorContext, text.str());
}

void ViewWindow::viewMoved()
{
    float x = m_glarea->getXoff();
    float y = m_glarea->getYoff();
    float z = m_glarea->getZoff();
    std::stringstream text;
    text << " " << x << "," << y << "," << z << " ";
    m_viewCoords->pop(m_viewContext);
    m_viewCoords->push(m_viewContext, text.str());
}

void ViewWindow::vAdjustChanged()
{
    std::cout << "North changed to " << m_vAdjust->get_value()
              << std::endl << std::flush;
    // m_glarea->setYoff(m_vAdjust->get_value());
    // m_glarea->redraw();
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    viewMoved();
}

void ViewWindow::hAdjustChanged()
{
    std::cout << "East changed to " << m_hAdjust->get_value()
              << std::endl << std::flush;
    // m_glarea->setXoff(-m_hAdjust->get_value());
    // m_glarea->redraw();
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    viewMoved();
}

void ViewWindow::glViewChanged()
{
    std::cout << "Changing sliders to take accout of view change"
              << std::endl << std::flush;
    float th, tv;
    m_glarea->getViewOffset(th, tv, m_dAdjust);
    m_hAdjust->set_value(-th);
    m_vAdjust->set_value(tv);
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ViewWindow.h"
#include "GlView.h"
#include "Model.h"

#include <gtkmm/frame.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/table.h>
#include <gtkmm/ruler.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>

#include <iostream>
#include <sstream>
#include <cmath>

ViewWindow::ViewWindow(MainWindow & w, Model & m) :
                                         Gtk::Window(Gtk::WINDOW_TOPLEVEL),
                                         m_glarea(NULL), m_mainWindow(w)
{
    m.nameChanged.connect(slot(*this, &ViewWindow::setTitle));
    // destroy.connect(slot(this, &ViewWindow::destroy_handler));

    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    // This needs to be done before the GlView is created
    m_cursorCoords = manage( new Gtk::Statusbar() );
    m_cursorContext = m_cursorCoords->get_context_id("Cursor coordinates");
    m_cursorCoords->push(" xxx,xxx,xxx ", m_cursorContext);

    m_viewCoords = manage( new Gtk::Statusbar() );
    m_viewContext = m_viewCoords->get_context_id("View coordinates");
    m_viewCoords->push(" xxx,xxx,xxx ", m_cursorContext);

    m_glarea = manage( new GlView(w, *this, m) );
    m_glarea->set_size_request(300,300);

    // Gtk::Frame * frame = manage( new Gtk::Frame() );
    // frame->set_shadow_type(GTK_SHADOW_IN);
    // frame->set_border_width(2);
    // frame->add(*m_glarea);

    // vbox->pack_start(*frame, true, true, 0);

    m_vAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_vAdjust->signal_value_changed().connect(slot(*this, &ViewWindow::vAdjustChanged));
    m_hAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_hAdjust->signal_value_changed().connect(slot(*this, &ViewWindow::hAdjustChanged));

    Gtk::Table * table = manage( new Gtk::Table(3, 3, false) );
    m_vRuler = manage( new Gtk::VRuler() );
    table->attach(*m_vRuler, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
    m_hRuler = manage( new Gtk::HRuler() );
    table->attach(*m_hRuler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
    m_vScrollbar = manage( new Gtk::VScrollbar(*m_vAdjust) );
    table->attach(*m_vScrollbar, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
    m_hScrollbar = manage( new Gtk::HScrollbar(*m_hAdjust) );
    table->attach(*m_hScrollbar, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);
    table->attach(*m_glarea, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
                                         Gtk::FILL | Gtk::EXPAND);

    vbox->pack_start(*table, Gtk::FILL | Gtk::EXPAND, 0);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );

    hbox->pack_start(*m_cursorCoords, Gtk::FILL | Gtk::EXPAND, 2);
    hbox->pack_start(*m_viewCoords, Gtk::FILL | Gtk::EXPAND, 2);

    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 0);

    add(*vbox);

    cursorMoved();
    viewMoved();

    show_all();

    m_glarea->viewChanged.connect(slot(*this, &ViewWindow::glViewChanged));
    glViewChanged();
    signal_delete_event().connect(slot(*this, &ViewWindow::deleteEvent));
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
    m_cursorCoords->push(text.str(), m_cursorContext);
}

void ViewWindow::viewMoved()
{
    float x = m_glarea->getXoff();
    float y = m_glarea->getYoff();
    float z = m_glarea->getZoff();
    std::stringstream text;
    text << " " << x << "," << y << "," << z << " ";
    m_viewCoords->pop(m_viewContext);
    m_viewCoords->push(text.str(), m_viewContext);
    doRulers();
}

void ViewWindow::vAdjustChanged()
{
    std::cout << "North changed to " << m_vAdjust->get_value()
              << std::endl << std::flush;
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    viewMoved();
}

void ViewWindow::hAdjustChanged()
{
    std::cout << "East changed to " << m_hAdjust->get_value()
              << std::endl << std::flush;
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    viewMoved();
}

void ViewWindow::glViewChanged()
{
    std::cout << "Changing sliders to take accout of view change"
              << std::endl << std::flush;
    float wx, wy, wz;
    m_glarea->m_model.getSize(wx, wy, wz);
    float winx = m_glarea->get_width() / (40.0f * m_glarea->getScale());
    float winy = m_glarea->get_height() / (40.0f * m_glarea->getScale());
    float worldSize = hypot(wx, wy);

    std::cout << "SLIDERS: " << winx << "," << winy << " (" << worldSize << ") "
              << std::endl << std::flush;

    m_hAdjust->set_lower(-worldSize);
    m_vAdjust->set_lower(-worldSize);
    m_hAdjust->set_upper(worldSize + winx);
    m_vAdjust->set_upper(worldSize + winx);
    
    m_hAdjust->set_page_size(winx);
    m_vAdjust->set_page_size(winx);

    m_hAdjust->set_page_increment(winx);
    m_vAdjust->set_page_increment(winy);

    m_hAdjust->set_step_increment(winx / 10.0f);
    m_vAdjust->set_step_increment(winy / 10.0f);

    float th, tv;
    m_glarea->getViewOffset(th, tv, m_dAdjust);
    m_hAdjust->set_value(-th);
    m_vAdjust->set_value(tv);

    doRulers();
}

void ViewWindow::doRulers()
{
    float th = m_hAdjust->get_value(), tv = m_vAdjust->get_value();
    float winx = m_glarea->get_width() / (40.0f * m_glarea->getScale());
    float winy = m_glarea->get_height() / (40.0f * m_glarea->getScale());
    float hrl = th - winx/2,
          hrh = th + winx/2,
          vrl = - tv + winy/2,
          vrh = - tv - winy/2;

    m_hRuler->set_range(hrl, hrh, th, hrh);
    m_hRuler->draw_ticks();
    m_vRuler->set_range(vrl, vrh, tv, vrh);
    m_vRuler->draw_ticks();
}

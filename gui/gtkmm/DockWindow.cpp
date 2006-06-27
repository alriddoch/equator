// Equator Online RPG World Building Tool
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "DockWindow.h"

#include "OptionBox.h"

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/tooltips.h>

DockWindow::DockWindow(OptionBox & ob) : m_vbox(0)
{
    set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);

    Gtk::Tooltips * ttips = manage( new Gtk::Tooltips() );

    m_vbox = manage( new Gtk::VBox() );
    add(*m_vbox);
    
    Gtk::Frame * frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(Gtk::SHADOW_IN);
    frame->set_size_request(5, 5);
    ttips->set_tip(*frame, "Top target");
    m_vbox->pack_start(*frame, Gtk::PACK_SHRINK);

    // We really need a VPaned here, in addition
    addChild(ob);

    frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(Gtk::SHADOW_IN);
    frame->set_size_request(5, 5);
    ttips->set_tip(*frame, "Bottom target");
    m_vbox->pack_end(*frame, Gtk::PACK_SHRINK);
}

void DockWindow::addChild(OptionBox & ob, bool top)
{
    m_vbox->pack_start(ob);
    ob.setDock(this);
    if (top) {
        m_vbox->reorder_child(ob, 0);
    }
}

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

#include "OptionBox.h"

#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>
#include <gtkmm/eventbox.h>

#include <cassert>

OptionBox::OptionBox(const Glib::ustring & title) : m_dock(0),
    m_targetList(1, Gtk::TargetEntry("application/x-equator-toolbar"))
{
    Gtk::HBox * tophbox = manage( new Gtk::HBox() );
    pack_start(*tophbox, Gtk::PACK_SHRINK, 6);

    Gtk::EventBox * eb = manage( new Gtk::EventBox() );
    Gtk::Label * l = manage( new Gtk::Label(title) );
    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    eb->add(*l);
    a->add(*eb);
    tophbox->pack_start(*a, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Button * b = manage( new Gtk::Button(Gtk::Stock::CLOSE) );
    b->set_label("");
    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*b);
    tophbox->pack_start(*a, Gtk::PACK_EXPAND_WIDGET);

    eb->drag_source_set(m_targetList, Gdk::ModifierType(GDK_BUTTON1_MASK),
                       Gdk::DragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
    eb->drag_source_set_icon(Gtk::Stock::DND);
}

void OptionBox::setDock(Gtk::Window * dock)
{
    if (dock == 0) {
        assert(m_dock != 0);
    } else {
        assert(m_dock == 0);
    }

    m_dock = dock;
}

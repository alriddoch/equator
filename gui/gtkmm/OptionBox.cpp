// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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

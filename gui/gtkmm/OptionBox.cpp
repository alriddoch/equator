// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "OptionBox.h"

#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>

OptionBox::OptionBox(const Glib::ustring & title) : m_dock(0),
    m_targetList(1, Gtk::TargetEntry("application/x-equator-toolbar", 0, 0))
{
    Gtk::HBox * tophbox = manage( new Gtk::HBox() );
    pack_start(*tophbox, Gtk::PACK_SHRINK, 6);

    Gtk::Label * l = manage( new Gtk::Label(title) );
    l->drag_source_set(m_targetList,
                       Gdk::ModifierType(GDK_BUTTON1_MASK | GDK_BUTTON3_MASK),
                       Gdk::DragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*l);
    tophbox->pack_start(*a, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Button * b = manage( new Gtk::Button(Gtk::Stock::CLOSE) );
    b->set_label("");
    a = manage( new Gtk::Alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*b);
    tophbox->pack_start(*a, Gtk::PACK_EXPAND_WIDGET);
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

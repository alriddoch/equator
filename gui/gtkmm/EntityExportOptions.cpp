// Equator Online RPG World Building Tool
// Copyright (C) 2000-2004 Alistair Riddoch
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

#include "EntityExportOptions.h"

#include <gtkmm/alignment.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/stock.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

EntityExportOptions::EntityExportOptions() : m_target(EXPORT_ALL),
                                             m_charType(NULL)
{
    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    Gtk::VBox * mainvbox = manage( new Gtk::VBox() );
    hbox->pack_start(*mainvbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*manage( new Gtk::Label("Export..") ) );
    mainvbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );
    mainvbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox() );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 6);

    Gtk::RadioButton * rb1 = manage( new Gtk::RadioButton("all entities") );
    rb1->signal_clicked().connect(sigc::bind<ExportTarget>(sigc::mem_fun(*this, &EntityExportOptions::setExportTarget),EXPORT_ALL));
    vbox->pack_start(*rb1, Gtk::PACK_SHRINK, 6);
    Gtk::RadioButton::Group rgp = rb1->get_group();
    Gtk::RadioButton * rb = manage( new Gtk::RadioButton(rgp, "visible entities") );
    rb->signal_clicked().connect(sigc::bind<ExportTarget>(sigc::mem_fun(*this, &EntityExportOptions::setExportTarget),EXPORT_VISIBLE));
    vbox->pack_start(*rb, Gtk::PACK_SHRINK, 6);
    rb = manage( new Gtk::RadioButton(rgp, "selected entity") );
    rb->signal_clicked().connect(sigc::bind<ExportTarget>(sigc::mem_fun(*this, &EntityExportOptions::setExportTarget),EXPORT_SELECTION));
    vbox->pack_start(*rb, Gtk::PACK_SHRINK, 6);
    rb = manage( new Gtk::RadioButton(rgp, "all selected entities") );
    rb->signal_clicked().connect(sigc::bind<ExportTarget>(sigc::mem_fun(*this, &EntityExportOptions::setExportTarget),EXPORT_ALL_SELECTED));
    vbox->pack_start(*rb, Gtk::PACK_SHRINK, 6);
    m_charCheck = manage( new Gtk::CheckButton("Remove characters") );
    vbox->pack_start(*m_charCheck, Gtk::PACK_SHRINK, 6);
    
    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*manage( new Gtk::Label("ID handling") ) );
    mainvbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );
    mainvbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox() );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 6);

    m_appendCheck = manage( new Gtk::CheckButton("Append") );
    vbox->pack_start(*m_appendCheck, Gtk::PACK_SHRINK, 6);

    hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);
    m_idSuffix = manage( new Gtk::Entry() );
    m_idSuffix->set_text("_map");
    hbox->pack_start(*m_idSuffix, Gtk::PACK_SHRINK, 6);

    Gtk::Label * t = manage( new Gtk::Label("to IDs in file.") );
    hbox->pack_start(*t, Gtk::PACK_SHRINK, 6);

    m_setRootCheck = manage( new Gtk::CheckButton("Set root id to ") );
    vbox->pack_start(*m_setRootCheck, Gtk::PACK_SHRINK, 6);

    hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);
    m_rootId = manage( new Gtk::Entry() );
    m_rootId->set_text("world_0");
    hbox->pack_start(*m_rootId, Gtk::PACK_SHRINK, 6);

    add_button(Gtk::Stock::CANCEL , Gtk::RESPONSE_CANCEL);
    m_ok = add_button(Gtk::Stock::OK , Gtk::RESPONSE_OK);

    signal_response().connect(sigc::mem_fun(*this, &EntityExportOptions::response));
}

void EntityExportOptions::setExportTarget(ExportTarget et)
{
    m_target = et;
}

void EntityExportOptions::response(int)
{
    hide();
}

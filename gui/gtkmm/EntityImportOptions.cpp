// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "EntityImportOptions.h"

#include <gtkmm/alignment.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/stock.h>

EntityImportOptions::EntityImportOptions() : m_target(IMPORT_TOPLEVEL)
{
    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox() );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT,
                                                    Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*manage( new Gtk::Label("Import entities into..") ) );
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox() );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 6);

    Gtk::RadioButton * rb1 = manage( new Gtk::RadioButton("top level") );
    rb1->signal_clicked().connect(SigC::bind<ImportTarget>(slot(*this, &EntityImportOptions::setImportTarget),IMPORT_TOPLEVEL));
    vbox->pack_start(*rb1, Gtk::PACK_SHRINK, 6);
    Gtk::RadioButton::Group rgp = rb1->get_group();
    Gtk::RadioButton * rb = manage( new Gtk::RadioButton(rgp, "selected entity") );
    rb->signal_clicked().connect(SigC::bind<ImportTarget>(slot(*this, &EntityImportOptions::setImportTarget),IMPORT_SELECTION));
    vbox->pack_start(*rb, Gtk::PACK_SHRINK, 6);

    add_button(Gtk::Stock::CANCEL , Gtk::RESPONSE_CANCEL);
    m_ok = add_button(Gtk::Stock::OK , Gtk::RESPONSE_OK);

    signal_response().connect(slot(*this, &EntityImportOptions::response));
}

void EntityImportOptions::setImportTarget(ImportTarget it)
{
    m_target = it;
}

void EntityImportOptions::response(int)
{
    hide();
}

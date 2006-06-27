// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

#include "CharacterWindow.h"

#include "Server.h"

#include <Eris/View.h>
#include <Eris/Avatar.h>
#include <Eris/Account.h>

#include <Atlas/Objects/Entity.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/combo.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/messagedialog.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/hide.h>

#include <iostream>
#include <cassert>

CharacterWindow::CharacterWindow() :
                 Gtk::Dialog("Create avatar on server", false, true),
                 m_nameEntry(0), m_typeEntry(0),
                 m_createButton(0), m_status(0),
                 m_server(0)
{
    m_takeButton = add_button("_Take Avatar", Gtk::RESPONSE_ACCEPT);
    m_takeButton->set_use_underline();
    m_createButton = add_button("_Create Avatar", Gtk::RESPONSE_YES);
    m_createButton->set_use_underline();
    add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
    set_default_response(Gtk::RESPONSE_YES);
    signal_response().connect(sigc::mem_fun(*this, &CharacterWindow::response));

    Gtk::VBox * vbox = get_vbox();

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    vbox->pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET, 6);

    vbox = manage( new Gtk::VBox(false, 6) );
    hbox->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET, 12);

    Gtk::Alignment * a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    m_serverLabel = manage( new Gtk::Label("Create avatar on XXX:"));
    a->add(*m_serverLabel);
    vbox->pack_start(*a);

    hbox = manage( new Gtk::HBox() );

    Gtk::Table * table = manage( new Gtk::Table(2, 3) );
    table->set_row_spacings(6);
    table->set_col_spacings(12);

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Name:") )));
    table->attach(*a, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_nameEntry = manage( new Gtk::Combo() );
    m_nameEntry->get_entry()->set_max_length(60);
    m_nameEntry->get_entry()->signal_key_press_event().connect(sigc::mem_fun(this, &CharacterWindow::nameKeyPressed), false);
    table->attach(*m_nameEntry, 1, 3, 0, 1);
    m_nameEntry->get_list()->signal_select_child().connect(sigc::mem_fun(*this, &CharacterWindow::select_child));
    m_nameEntry->get_list()->signal_selection_changed().connect(sigc::mem_fun(*this, &CharacterWindow::selection_changed));
    m_nameEntry->get_list()->signal_unselect_child().connect(sigc::mem_fun(*this, &CharacterWindow::unselect_child));

    a = manage( new Gtk::Alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0, 0) );
    a->add(*(manage( new Gtk::Label("Type:") )));
    table->attach(*a, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 6);
    m_typeEntry = manage( new Gtk::Entry() );
    m_typeEntry->set_max_length(60);
    m_typeEntry->set_activates_default();
    table->attach(*m_typeEntry, 1, 3, 1, 2);

    hbox->pack_start(*table);

    vbox->pack_start(*hbox);

    m_status = manage( new Gtk::Statusbar() );
    m_statusContext = m_status->get_context_id("Login status");
    // vbox->pack_start(*m_status, Gtk::PACK_SHRINK, 0);

    signal_delete_event().connect(sigc::mem_fun(*this, &CharacterWindow::deleteEvent));
}

void CharacterWindow::doshow()
{
    m_nameEntry->get_entry()->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);
    show_all();
}

void CharacterWindow::useServer(Server * s)
{
    assert(s != 0);

    m_server = s;
    m_serverLabel->set_text(std::string("Create avatar on ") + m_server->getName() + ":");
    m_charlist = m_server->m_account->GotAllCharacters.connect(sigc::mem_fun(*this, &CharacterWindow::gotCharacterList));
    m_server->m_account->refreshCharacterInfo();

    std::list<Glib::ustring> listStrings;
    m_nameEntry->set_popdown_strings(listStrings);
    m_selectedCharacter.clear();
    m_takeButton->set_sensitive(false);
    m_createButton->set_sensitive(true);
}

void CharacterWindow::gotCharacterList()
{
    assert(m_server != 0);

    std::cout << "GOT CHARACTER LIST" << std::endl << std::flush;

    const Eris::CharacterMap & chars = m_server->m_account->getCharacters();
    Eris::CharacterMap::const_iterator I = chars.begin();
    // std::list<Glib::ustring> listStrings;
    for(; I != chars.end(); ++I) {
        const std::string & id = I->first;
        const Atlas::Objects::Entity::RootEntity & ge = I->second;
        Gtk::ComboDropDownItem* item = Gtk::manage(new Gtk::ComboDropDownItem);
        Gtk::HBox * hbox = manage(new Gtk::HBox(false, 3));
        Gtk::Label * l = manage(new Gtk::Label());
        l->set_markup(ge->getName() + " <i>(" + id + ")</i>");
        hbox->pack_start(*l, Gtk::PACK_SHRINK);
        item->add(*hbox);
        item->show_all();
        m_nameEntry->get_list()->children().push_back(*item);
        m_nameEntry->set_item_string(*item, id);
        // listStrings.push_back(ge.getName());
    }
    // m_nameEntry->set_popdown_strings(listStrings);
}

void CharacterWindow::select_child(Gtk::Widget & w)
{
    if (m_server == 0) {
        return;
    }

    m_selectedCharacter.clear();
    std::string id = m_nameEntry->get_entry()->get_text();
    std::cout << "Selected " << id << std::endl << std::flush;
    const Eris::CharacterMap & chars = m_server->m_account->getCharacters();
    Eris::CharacterMap::const_iterator I = chars.find(id);
    if (I == chars.end()) {
        return;
    }
    const Atlas::Objects::Entity::RootEntity & ge = I->second;
    m_selectedCharacter = id;
    m_nameEntry->get_entry()->set_text(ge->getName());
    m_typeEntry->set_text(ge->getParents().front());
    m_typeEntry->set_editable(false);
    m_takeButton->set_sensitive(true);
    set_default_response(Gtk::RESPONSE_ACCEPT);
    m_createButton->set_sensitive(false);
    return;
}

void CharacterWindow::selection_changed()
{
}

bool CharacterWindow::nameKeyPressed(GdkEventKey * pEvent)
{
    if(pEvent->keyval == GDK_Return)
    {
        activate_default();
        
        return true;
    }
    
    return false;
}

void CharacterWindow::unselect_child(Gtk::Widget&)
{
    m_typeEntry->set_editable(true);
    m_selectedCharacter.clear();
    m_takeButton->set_sensitive(false);
    m_createButton->set_sensitive(true);
    set_default_response(Gtk::RESPONSE_YES);
}

void CharacterWindow::take()
{
    assert(m_server != 0);

    m_status->push("Taking character...", m_statusContext);

    m_nameEntry->get_entry()->set_editable(false);
    m_typeEntry->set_editable(false);
    m_createButton->set_sensitive(true);

    m_server->takeCharacter(m_selectedCharacter);

    m_success = m_server->m_account->AvatarSuccess.connect(sigc::mem_fun(*this, &CharacterWindow::success));
    m_failure = m_server->m_account->AvatarFailure.connect(sigc::mem_fun(*this, &CharacterWindow::failure));
}

void CharacterWindow::create()
{
    assert(m_server != 0);

    m_status->push("Creating character...", m_statusContext);

    m_nameEntry->get_entry()->set_editable(false);
    m_typeEntry->set_editable(false);
    m_createButton->set_sensitive(false);

    m_server->createCharacter(m_nameEntry->get_entry()->get_text(), m_typeEntry->get_text());

    m_success = m_server->m_account->AvatarSuccess.connect(sigc::mem_fun(*this, &CharacterWindow::success));
    m_failure = m_server->m_account->AvatarFailure.connect(sigc::mem_fun(*this, &CharacterWindow::failure));
}

void CharacterWindow::failure(const std::string & msg)
{
    assert(m_server != 0);

    Gtk::MessageDialog * md = new Gtk::MessageDialog(*this, msg, false, Gtk::MESSAGE_ERROR);
    md->signal_response().connect(sigc::hide(sigc::mem_fun(*md, &Gtk::MessageDialog::hide)));
    md->show();

    std::cout << "CharacterWindow::failure" << std::endl << std::flush;
    std::cout << msg << std::endl << std::flush;

    m_status->pop(m_statusContext);
    m_status->push("Connection failed", m_statusContext);

    m_failure.disconnect();
    m_success.disconnect();
    m_charlist.disconnect();

    m_nameEntry->get_entry()->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);
}

void CharacterWindow::success(Eris::Avatar *)
{
    assert(m_server != 0);

    m_status->pop(m_statusContext);
    m_status->push("Logged In", m_statusContext);

    m_server = 0;

    m_nameEntry->get_entry()->set_editable(true);
    m_typeEntry->set_editable(true);
    m_createButton->set_sensitive(true);

    std::cout << "Got character success in CharacterWindow" << std::endl
              << std::flush;

    m_failure.disconnect();
    m_success.disconnect();
    m_charlist.disconnect();

    hide();
}

void CharacterWindow::response(int response)
{
    if (response == Gtk::RESPONSE_ACCEPT) {
        take();
    } else if (response == Gtk::RESPONSE_YES) {
        create();
    } else if (response == Gtk::RESPONSE_CLOSE) {
        m_failure.disconnect();
        m_success.disconnect();
        m_charlist.disconnect();
        hide();
    }
}

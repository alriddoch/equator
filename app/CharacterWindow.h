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

#ifndef EQUATOR_APP_CHARACTER_WINDOW_H
#define EQUATOR_APP_CHARACTER_WINDOW_H

#include <gtkmm/dialog.h>

class Server;

namespace Gtk {
  class Label;
  class Combo;
  class Entry;
  class Button;
  class Statusbar;
}

namespace Eris {
  class Avatar;
}

class CharacterWindow : public Gtk::Dialog
{
  private:
    Gtk::Label * m_serverLabel;
    Gtk::Combo * m_nameEntry;
    Gtk::Entry * m_typeEntry;
    Gtk::Button * m_takeButton;
    Gtk::Button * m_createButton;
    Gtk::Statusbar * m_status;

    int m_statusContext;
    Server * m_server;
    std::string m_selectedCharacter;

    sigc::connection m_failure;
    sigc::connection m_success;
    sigc::connection m_charlist;

    void gotCharacterList();
    void select_child(Gtk::Widget&);
    void selection_changed();
    void unselect_child(Gtk::Widget&);

    void create();
    void take();

  public:
    CharacterWindow();

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();
    bool nameKeyPressed(GdkEventKey * pEvent);

    void useServer(Server *);

    void failure(const std::string & msg);
    void success(Eris::Avatar *);

    void response(int);
};

#endif // EQUATOR_APP_CHARACTER_WINDOW_H

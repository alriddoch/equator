// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_CHARACTER_WINDOW_H
#define EQUATOR_APP_CHARACTER_WINDOW_H

#include <gtkmm/dialog.h>

class Server;

namespace Gtk {
  class Entry;
  class Combo;
  class Button;
  class Statusbar;
  class Label;
}

namespace Eris {
  class Entity;
}

class CharacterWindow : public Gtk::Dialog
{
  private:
    Gtk::Label * m_serverLabel;
    Gtk::Combo * m_nameEntry;
    Gtk::Entry * m_typeEntry;
    Gtk::Button * m_createButton;

    Gtk::Statusbar * m_status;
    int m_statusContext;
    Server * m_server;

    SigC::Connection m_failure;
    SigC::Connection m_created;
    SigC::Connection m_charlist;

    void gotCharacterList();
    void select_child(Gtk::Widget&);
    void selection_changed();
    void unselect_child(Gtk::Widget&);
  public:
    CharacterWindow();

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow();

    void useServer(Server *);

    void create();

    void failure(const std::string & msg);
    void created(Eris::Entity *);

    void dismiss(int);

    SigC::Signal1<void, Server *> createSuccess;
};

#endif // EQUATOR_APP_CHARACTER_WINDOW_H

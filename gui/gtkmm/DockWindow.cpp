// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "DockWindow.h"

#include "OptionBox.h"

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/tooltips.h>

DockWindow::DockWindow(OptionBox & ob)
{
    set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY);

    Gtk::Tooltips * ttips = manage( new Gtk::Tooltips() );

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    add(*vbox);
    
    Gtk::Frame * frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(Gtk::SHADOW_IN);
    frame->set_size_request(5, 5);
    ttips->set_tip(*frame, "Top target");
    vbox->pack_start(*frame, Gtk::PACK_SHRINK);

    // We really need a VPaned here, in addition
    vbox->pack_start(ob);

    frame = manage( new Gtk::Frame() );
    frame->set_shadow_type(Gtk::SHADOW_IN);
    frame->set_size_request(5, 5);
    ttips->set_tip(*frame, "Bottom target");
    vbox->pack_end(*frame, Gtk::PACK_SHRINK);
}

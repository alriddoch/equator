// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "InheritanceWindow.h"
#include "AtlasMapWidget.h"
#include "inheritance.h"

#include <gtk--/frame.h>
#include <gtk--/menuitem.h>
#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/button.h>
#include <gtk--/separator.h>
#include <gtk--/tree.h>
#include <gtk--/ctree.h>

#include <iostream>
#include <vector>

using Atlas::Message::Object;

InheritanceWindow::InheritanceWindow(MainWindow & mw) :
                                     Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                     m_mainWindow(mw)
{
    // destroy.connect(slot(this, &InheritanceWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Inheritance tree:") ) ), false, false, 2);
    m_connectionLabel = manage( new Gtk::Label("-unconnected-") );
    tophbox->pack_start(*m_connectionLabel, true, false, 2);
    tophbox->pack_end(*(manage( new Gtk::Label("WOOT") ) ), false, false, 2);
   
    vbox->pack_start(*tophbox, false, false, 2);

    m_classTree = manage( new Gtk::Tree() );
    
    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // FIXME Add this in a way that won't cause an error
    scrolled_window->add_with_viewport(*m_classTree);
    // scrolled_window->add(*m_classTree);

    vbox->pack_start(*scrolled_window, true, true, 2);

    static const gchar *titles[] = { "Name", "Attribute", NULL };
    Atlas::Message::Object::MapType test;
    test["Footle"] = Atlas::Message::Object::ListType(1, "foo");
    test["foo"] = 23;
    test["bar"] = 0.1;
    test["baz"] = "hello";
    test["mim"] = test;
    test["grep"] = test;
    m_attributeTree = manage( new AtlasMapWidget(titles, test) );
    m_attributeTree->set_column_width (0, 100);
    m_attributeTree->set_column_width (1, 100);

    vbox->pack_start(*manage(new Gtk::HSeparator()), false, false, 0);

    scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    scrolled_window->add(*m_attributeTree);

    vbox->pack_start(*scrolled_window, true, true, 2);

    Gtk::HBox * bothbox = manage( new Gtk::HBox() );
    //Gtk::Button * b = manage( new Gtk::Button("New...") );
    //b->clicked.connect(slot(this, &InheritanceWindow::newLayer));
    //bothbox->pack_start(*b, true, true, 0);

    vbox->pack_end(*bothbox, false, true, 2);

    add(*vbox);
    set_title("Inheritance");

    // show_all();
}

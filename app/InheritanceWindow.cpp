// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "InheritanceWindow.h"
#include "AtlasMapWidget.h"
#include "inheritance.h"

#include <gtkmm/frame.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include <iostream>
#include <vector>

using Atlas::Message::Object;

InheritanceWindow::InheritanceWindow(MainWindow & mw) :
                                     Gtk::Window(Gtk::WINDOW_TOPLEVEL),
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

    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_valueColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_valueColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_nameColumn] = Glib::ustring("test name");
    row[*m_valueColumn] = Glib::ustring("test value");

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );
    
    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    // FIXME Add this in a way that won't cause an error
    scrolled_window->add(*m_treeView);
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
    m_attributeTree = manage( new AtlasMapWidget(/* titles, */ test) );
    // m_attributeTree->set_column_width (0, 100);
    // m_attributeTree->set_column_width (1, 100);

    vbox->pack_start(*manage(new Gtk::HSeparator()), false, false, 0);

    scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
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
    signal_delete_event().connect(slot(*this, &InheritanceWindow::deleteEvent));
}

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

#include "InheritanceWindow.h"

#include "MainWindow.h"
#include "Server.h"
#include "AtlasMapWidget.h"

#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeService.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <cassert>

using Atlas::Message::Element;

InheritanceWindow::InheritanceWindow(MainWindow & mw): OptionBox("Inheritance"),
                                                       m_currentServer(0),
                                                       m_mainWindow(mw)
{
    // destroy.connect(slot(this, &InheritanceWindow::destroy_handler));
    // Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );
    Gtk::VBox * vbox = this;

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") ) ), Gtk::PACK_SHRINK, 2);
    m_serverMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_serverMenu, Gtk::PACK_EXPAND_WIDGET, 2);
    tophbox->pack_start(*(manage( new Gtk::Label("WOOT") ) ), Gtk::PACK_SHRINK, 2);
   
    vbox->pack_start(*tophbox, Gtk::PACK_SHRINK, 2);

    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    // m_valueColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_columns->add(*m_nameColumn);
    // m_columns->add(*m_valueColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_nameColumn] = "test name";
    // row[*m_valueColumn] = Glib::ustring("test value");

    Gtk::TreeModel::Row childrow = *(m_treeModel->append(row.children()));
    childrow[*m_nameColumn] = "test child";

    Gtk::TreeModel::Row grandchildrow = *(m_treeModel->append(childrow.children()));
    grandchildrow[*m_nameColumn] = "test child";

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("name", *m_nameColumn);
    // m_treeView->append_column("name", *m_valueColumn);
    
    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    // FIXME Add this in a way that won't cause an error
    scrolled_window->add(*m_treeView);
    // scrolled_window->add(*m_classTree);

    vbox->pack_start(*scrolled_window, Gtk::PACK_EXPAND_WIDGET, 2);

    Atlas::Message::MapType test;
    test["Footle"] = Atlas::Message::ListType(1, "foo");
    test["foo"] = 23;
    test["bar"] = 0.1;
    test["baz"] = "hello";
    test["mim"] = Atlas::Message::ListType(1, 235);
    test["grep"] = Atlas::Message::ListType(1, 0.4556);
    m_attributeTree = manage( new AtlasMapWidget(/* titles, */ test) );
    // m_attributeTree->set_column_width (0, 100);
    // m_attributeTree->set_column_width (1, 100);

    vbox->pack_start(*manage(new Gtk::HSeparator()), Gtk::PACK_SHRINK, 0);

    scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrolled_window->add(*m_attributeTree);

    vbox->pack_start(*scrolled_window, Gtk::PACK_EXPAND_WIDGET, 2);

    Gtk::HBox * bothbox = manage( new Gtk::HBox() );
    //Gtk::Button * b = manage( new Gtk::Button("New...") );
    //b->clicked.connect(slot(this, &InheritanceWindow::newLayer));
    //bothbox->pack_start(*b, true, true, 0);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 2);

    // add(*vbox);
    // set_title("Inheritance");
    set_sensitive(false);

    mw.serverAdded.connect(sigc::mem_fun(*this, &InheritanceWindow::serverAdded));

    signal_delete_event().connect(sigc::mem_fun(*this, &InheritanceWindow::deleteEvent));
}

void InheritanceWindow::descendTypeTree(Eris::TypeInfo * node,
                                        Gtk::TreeModel::Row & row)
{
    assert(node != NULL);

    std::cout << "Node " << node->getName() << std::endl << std::flush;
    row[*m_nameColumn] = node->getName();
    const Eris::TypeInfoSet & children = node->getChildren();
    Eris::TypeInfoSet::const_iterator I = children.begin();
    for (; I != children.end(); I++) {
        Gtk::TreeModel::Row childrow = *(m_treeModel->append(row.children()));
        descendTypeTree(*I, childrow);
    }

}

void InheritanceWindow::currentServerChanged(Server * s)
{
    if (s == m_currentServer) {
        return;
    }

    m_currentServer = s;
    m_treeModel->clear();

    if (m_currentServer == 0) {
        set_sensitive(false);
        return;
    }

    set_sensitive(true);
    if (s->m_connection == 0) {
        std::cout << "FIXME: Server not yet connected." << std::endl << std::flush;
        return;
    }

    Eris::TypeService * ts = s->m_connection->getTypeService();

    if (ts == 0) {
        std::cout << "No type service)" << std::endl << std::flush;
        return;
    }

    Eris::TypeInfoPtr root = ts->findTypeByName("root");

    if (root == 0) {
        std::cout << "No root type)" << std::endl << std::flush;
        return;
    }
    Gtk::TreeModel::Row row = *(m_treeModel->append());
    descendTypeTree(root, row);
}

void InheritanceWindow::serverAdded(Server * s)
{
    std::cout << "SERVER ADDED" << std::endl << std::flush;

    Gtk::Menu * menu = m_serverMenu->get_menu();
    bool newMenu = false;

    if (menu == 0) {
        newMenu = true;
        menu = manage( new Gtk::Menu() );
        m_serverMenu->set_menu(*menu);
    }
    Gtk::Menu_Helpers::MenuList & server_menu = menu->items();
    std::stringstream ident;
    ident << s->getName() << "-" << s->getServerNo();

    server_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), sigc::bind<Server*>(sigc::mem_fun(*this, &InheritanceWindow::currentServerChanged), s)));
    if (newMenu) {
        m_serverMenu->set_history(0);
        currentServerChanged(s);
    }
}

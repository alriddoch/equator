// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "EntityTree.h"

#include "app/MainWindow.h"
#include "app/Model.h"
#include "app/Server.h"
#include "app/ServerEntities.h"
#include "app/AtlasMapWidget.h"

#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/TypeInfo.h>

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

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <iostream>
#include <sstream>
#include <vector>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

EntityTree::EntityTree(MainWindow & mw): OptionBox("Entity Tree"),
                                                       m_currentModel(0),
                                                       m_mainWindow(mw)
{
    // destroy.connect(slot(this, &EntityTree::destroy_handler));
    // Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );
    Gtk::VBox * vbox = this;

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") ) ), Gtk::PACK_SHRINK, 2);
    m_modelMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_modelMenu, Gtk::PACK_EXPAND_WIDGET, 2);
    tophbox->pack_start(*(manage( new Gtk::Label("WOOT") ) ), Gtk::PACK_SHRINK, 2);
   
    vbox->pack_start(*tophbox, Gtk::PACK_SHRINK, 2);

    m_columns = new Gtk::TreeModelColumnRecord();
    m_idColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_typeColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_columns->add(*m_idColumn);
    m_columns->add(*m_typeColumn);
    m_columns->add(*m_nameColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_idColumn] = "test id";
    row[*m_typeColumn] = "test type";
    row[*m_nameColumn] = "test name";

    Gtk::TreeModel::Row childrow = *(m_treeModel->append(row.children()));
    childrow[*m_idColumn] = "test child id";
    childrow[*m_typeColumn] = "test child type";
    childrow[*m_nameColumn] = "test child name";

    Gtk::TreeModel::Row grandchildrow = *(m_treeModel->append(childrow.children()));
    grandchildrow[*m_idColumn] = "test grandchild id";
    grandchildrow[*m_typeColumn] = "test grandchild type";
    grandchildrow[*m_nameColumn] = "test grandchild";

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("ID", *m_idColumn);
    m_treeView->append_column("type", *m_typeColumn);
    m_treeView->append_column("name", *m_nameColumn);
    
    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    // FIXME Add this in a way that won't cause an error
    scrolled_window->add(*m_treeView);
    // scrolled_window->add(*m_classTree);

    vbox->pack_start(*scrolled_window, Gtk::PACK_EXPAND_WIDGET, 2);

    MapType test;
    test["Footle"] = ListType(1, "foo");
    test["foo"] = 23;
    test["bar"] = 0.1;
    test["baz"] = "hello";
    test["mim"] = ListType(1, 235);;
    test["grep"] = ListType(1, 0.2342);;
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
    //b->clicked.connect(slot(this, &EntityTree::newLayer));
    //bothbox->pack_start(*b, true, true, 0);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 2);

    // add(*vbox);
    // set_title("Inheritance");
    set_sensitive(false);

    mw.modelAdded.connect(SigC::slot(*this, &EntityTree::modelAdded));

    signal_delete_event().connect(slot(*this, &EntityTree::deleteEvent));
}

void EntityTree::descendEntityTree(Eris::Entity * node,
                                   Gtk::TreeModel::Row & row)
{
    assert(node != NULL);

    std::cout << "Node " << node->getName() << std::endl << std::flush;
    row[*m_idColumn] = node->getId();
    row[*m_typeColumn] = node->getType()->getName();
    row[*m_nameColumn] = node->getName();

    int numEnts = node->numContained();
    for (int i = 0; i < numEnts; ++i) {
        Eris::Entity * child = node->getContained(i);
        Gtk::TreeModel::Row childrow = *(m_treeModel->append(row.children()));
        descendEntityTree(child, childrow);
    }

}

void EntityTree::currentModelChanged(Model * m)
{
    if (m == m_currentModel) {
        return;
    }

    m_currentModel = m;
    m_treeModel->clear();

    if (m_currentModel == 0) {
        set_sensitive(false);
        return;
    }

    set_sensitive(true);
    const std::list<Layer *> & layers = m_currentModel->getLayers();
    ServerEntities * se = 0;
    std::list<Layer *>::const_iterator I = layers.begin();
    for(; I != layers.end() && se == 0; ++I) {
        se = dynamic_cast<ServerEntities *>(*I);
    }

    if (se == 0) {
        std::cout << "No ServerEntities" << std::endl << std::flush;
        return;
    }

    Server & server = se->m_serverConnection;

    if (!server.isInGame()) {
        std::cout << "Not In-Game" << std::endl << std::flush;
        return;
    }

    assert(server.m_view != 0);

    Eris::Entity * root = server.m_view->getTopLevel();

    if (root == 0) {
        std::cout << "No root entity" << std::endl << std::flush;
        return;
    }
    Gtk::TreeModel::Row row = *(m_treeModel->append());
    descendEntityTree(root, row);
}

void EntityTree::modelAdded(Model * s)
{
    std::cout << "EntityTree: MODEL ADDED" << std::endl << std::flush;

    Gtk::Menu * menu = m_modelMenu->get_menu();
    bool newMenu = false;

    if (menu == 0) {
        newMenu = true;
        menu = manage( new Gtk::Menu() );
        m_modelMenu->set_menu(*menu);
    }
    Gtk::Menu_Helpers::MenuList & model_menu = menu->items();
    std::stringstream ident;
    ident << s->getName() << "-" << s->getModelNo();

    model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(SigC::slot(*this, &EntityTree::currentModelChanged), s)));
    if (newMenu) {
        m_modelMenu->set_history(0);
        currentModelChanged(s);
    }
}

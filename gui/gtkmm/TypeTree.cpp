// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TypeTree.h"

#include "app/Server.h"

#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeselection.h>

#include <sigc++/object_slot.h>

#include <iostream>
#include <vector>

#include <cassert>

TypeTree::TypeTree(Server & s) : OptionBox("Type Tree"), m_server(s)
{
    Gtk::VBox * vbox = this;

    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_ptrColumn = new Gtk::TreeModelColumn<Eris::TypeInfo *>();
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_ptrColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("Typename", *m_nameColumn);

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    // m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &TypeTree::selectionChanged) );

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);

    Gtk::HBox * bothbox = manage( new Gtk::HBox );

    Gtk::Button * b = manage( new Gtk::Button("Wuh...") );
    // b->signal_clicked().connect(SigC::slot(*this, &TypeTree::typesPressed));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 6);

    signal_delete_event().connect(SigC::slot(*this, &TypeTree::deleteEvent));
}

void TypeTree::insertType(Eris::TypeInfo * const ti, Gtk::TreeModel::Row row)
{
    assert(ti != 0);


    row[*m_nameColumn] = Glib::ustring(ti->getName());
    row[*m_ptrColumn] = ti;

    const Eris::TypeInfoSet & children = ti->getChildren();
    Eris::TypeInfoSet::const_iterator I = children.begin();
    Eris::TypeInfoSet::const_iterator Iend = children.end();
    for (; I != Iend; ++I) {
        Gtk::TreeModel::Row childrow = *(m_treeModel->append(row.children()));
        insertType(*I, childrow);
    }
}

void TypeTree::populate()
{
    Eris::TypeService * ts = m_server.m_connection.getTypeService();

    assert(ts != 0);

    Eris::TypeInfo * ti = ts->findTypeByName("root");
    if (ti == 0) {
        std::cout << "No types" << std::endl << std::flush;
    }

    Gtk::TreeModel::Row row = *(m_treeModel->append());

    insertType(ti, row);
}

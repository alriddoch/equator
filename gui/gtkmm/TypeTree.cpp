// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "TypeTree.h"

#include "app/Server.h"

#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/menu.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/treeview.h>
#include <gtkmm/uimanager.h>

#include <sigc++/object_slot.h>

#include <iostream>
#include <vector>

#include <cassert>

#include "ImportTypesWizard.h"

static const Glib::ustring g_sUI = ""
"<ui>"
"  <popup name='PopupMenu'>"
"    <menuitem action='Import'/>"
"  </popup>"
"  <toolbar name='ToolBar'>"
"    <toolitem action='Import'/>"
"  </toolbar>"
"</ui>";

TypeTree::TypeTree(Server & s) : OptionBox("Type Tree"), m_server(s),
    m_pImportTypesWizard(0)
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
    m_treeView->signal_button_press_event().connect(sigc::mem_fun(this, &TypeTree::buttonPressEvent), false);

    m_treeView->append_column("Typename", *m_nameColumn);

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    // m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &TypeTree::selectionChanged) );

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);
    m_actionImport = Gtk::Action::create("Import", "Import ...", "Import type definitions from a file.");
    m_actions = Gtk::ActionGroup::create();
    m_actions->add(m_actionImport, sigc::mem_fun(this, &TypeTree::importPressed));
    m_UIManager = Gtk::UIManager::create();
    m_UIManager->insert_action_group(m_actions);
    m_UIManager->add_ui_from_string(g_sUI);
    m_popupMenu = dynamic_cast< Gtk::Menu * >(m_UIManager->get_widget("/PopupMenu"));
    vbox->pack_start(*(m_UIManager->get_widget("/ToolBar")), Gtk::PACK_SHRINK, 0);
    signal_delete_event().connect(SigC::slot(*this, &TypeTree::deleteEvent));
}

bool TypeTree::buttonPressEvent(GdkEventButton * pEvent)
{
    if (pEvent->button == 3)
    {
        m_popupMenu->popup(pEvent->button, pEvent->time);
    }
    
    return false;
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

void TypeTree::importPressed()
{
    if(m_pImportTypesWizard == 0)
    {
        m_pImportTypesWizard = new ImportTypesWizard();
        m_pImportTypesWizard->signal_response().connect(sigc::mem_fun(this, &TypeTree::importTypesWizardResponse));
    }
    m_pImportTypesWizard->present();
}

void TypeTree::importTypesWizardResponse(int iResponse)
{
    if(iResponse == Gtk::RESPONSE_OK)
    {
        // to the import
    }
    if((iResponse == Gtk::RESPONSE_OK) || (iResponse == Gtk::RESPONSE_CANCEL))
    {
        // only close the wizard on Cancel or OK
        delete m_pImportTypesWizard;
        m_pImportTypesWizard = 0;
    }
}

void TypeTree::populate()
{
    Eris::TypeService * ts = m_server.m_connection->getTypeService();

    assert(ts != 0);

    Eris::TypeInfo * ti = ts->findTypeByName("root");
    if (ti == 0) {
        std::cout << "No types" << std::endl << std::flush;
    }

    Gtk::TreeModel::Row row = *(m_treeModel->append());

    insertType(ti, row);
}

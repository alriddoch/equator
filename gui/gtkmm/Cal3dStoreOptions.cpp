// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Cal3dStoreOptions.h"

#include "app/Cal3dStore.h"
#include "app/Model.h"

#include "visual/Model.h"

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

Cal3dStoreOptions::Cal3dStoreOptions(Cal3dStore & s) :
                                     OptionBox("Cal3d Options"),
                                     m_cal3dStore(s)
{
    Gtk::VBox * vbox = this;

    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_selectColumn = new Gtk::TreeModelColumn<bool>();
    m_idColumn = new Gtk::TreeModelColumn<int>();
    m_componentColumn = new Gtk::TreeModelColumn<ComponentType>();
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_selectColumn);
    m_columns->add(*m_idColumn);
    m_columns->add(*m_componentColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    m_treeView->append_column("Component", *m_nameColumn);

    Gtk::CellRendererToggle * crt = manage( new Gtk::CellRendererToggle() );
    crt->signal_toggled().connect( SigC::slot(*this, &Cal3dStoreOptions::enableToggled) );
    int column_no = m_treeView->append_column("View", *crt);
    Gtk::TreeViewColumn * column = m_treeView->get_column(column_no - 1);
    column->add_attribute(crt->property_active(), *m_selectColumn);
    column->set_clickable();

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    // m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &Cal3dStoreOptions::selectionChanged) );

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);

    Gtk::HBox * bothbox = manage( new Gtk::HBox );

    Gtk::Button * b = manage( new Gtk::Button("Wuh...") );
    // b->signal_clicked().connect(SigC::slot(*this, &Cal3dStoreOptions::typesPressed));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 6);

    animationRow = *(m_treeModel->append());
    animationRow[*m_nameColumn] = Glib::ustring("Animations");

    actionRow = *(m_treeModel->append());
    actionRow[*m_nameColumn] = Glib::ustring("Actions");

    meshRow = *(m_treeModel->append());
    meshRow[*m_nameColumn] = Glib::ustring("Meshes");

    materialRow = *(m_treeModel->append());
    materialRow[*m_nameColumn] = Glib::ustring("Materials");

    Cal3dModel & m = s.getModel();

    m.skeletonLoaded.connect(SigC::slot(*this, &Cal3dStoreOptions::skeletonLoaded));
    m.animationLoaded.connect(SigC::slot(*this, &Cal3dStoreOptions::animationLoaded));
    m.actionLoaded.connect(SigC::slot(*this, &Cal3dStoreOptions::actionLoaded));
    m.meshLoaded.connect(SigC::slot(*this, &Cal3dStoreOptions::meshLoaded));
    m.materialLoaded.connect(SigC::slot(*this, &Cal3dStoreOptions::materialLoaded));
}

void Cal3dStoreOptions::skeletonLoaded(const std::string & s)
{
    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_nameColumn] = Glib::ustring(s);
    row[*m_componentColumn] = COMP_SKELETON;
}

void Cal3dStoreOptions::animationLoaded(const std::string & s, int id)
{
    Gtk::TreeModel::Row row = *(m_treeModel->append(animationRow.children()));
    row[*m_nameColumn] = Glib::ustring(s);
    row[*m_idColumn] = id;
    row[*m_componentColumn] = COMP_ANIMATION;
}

void Cal3dStoreOptions::actionLoaded(const std::string & s, int id)
{
    Gtk::TreeModel::Row row = *(m_treeModel->append(actionRow.children()));
    row[*m_nameColumn] = Glib::ustring(s);
    row[*m_idColumn] = id;
    row[*m_componentColumn] = COMP_ACTION;
}

void Cal3dStoreOptions::meshLoaded(const std::string & s, int id)
{
    bool enabled = false;
    if (m_cal3dStore.getModel().enabledMeshes().find(id) !=
        m_cal3dStore.getModel().enabledMeshes().end()) {
        enabled = true;
        std::cout << "Enabled " << std::endl << std::flush;
    } else {
        std::cout << "Not Enabled " << std::endl << std::flush;
    }

    Gtk::TreeModel::Row row = *(m_treeModel->append(meshRow.children()));
    row[*m_nameColumn] = Glib::ustring(s);
    row[*m_selectColumn] = enabled;
    row[*m_idColumn] = id;
    row[*m_componentColumn] = COMP_MESH;

}

void Cal3dStoreOptions::materialLoaded(const std::string & s, int id)
{
    Gtk::TreeModel::Row row = *(m_treeModel->append(materialRow.children()));
    row[*m_nameColumn] = Glib::ustring(s);
    row[*m_idColumn] = id;
    row[*m_componentColumn] = COMP_MATERIAL;
}

void Cal3dStoreOptions::enableToggled(const Glib::ustring& path_string)
{
    GtkTreePath *gpath = gtk_tree_path_new_from_string (path_string.c_str());
    Gtk::TreePath path(gpath);

    /* get toggled iter */
    Gtk::TreeRow row = *(m_treeModel->get_iter(path));

    bool enabled = row[*m_selectColumn];
    int id = row[*m_idColumn];
    ComponentType ct = row[*m_componentColumn];
    Cal3dModel & model = m_cal3dStore.getModel();

    switch (ct) {
      case COMP_MESH:
        { 
          if (enabled) {
              model.enabledMeshes().erase(id);
          } else {
              model.enabledMeshes().insert(id);
          }
          row[*m_selectColumn] = !enabled;
          m_cal3dStore.model().updated.emit();
        }
        default:
            break;
    };
    // /* do something with the value */
    // visible = layer->toggleVisible();

    // /* set new value */
    // row[*m_visColumn] = visible;

    // if (0 != m_currentModel) {
        // m_currentModel->updated.emit();
    // }
}

#if 0
void Cal3dOptions::insertType(Eris::TypeInfo * const ti, Gtk::TreeModel::Row row)
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

void Cal3dOptions::populate()
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
#endif

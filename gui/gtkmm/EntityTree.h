// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_ENTITY_TREE_H
#define EQUATOR_GUI_GTKMM_ENTITY_TREE_H

#include "OptionBox.h"

#include <gtkmm/treestore.h>

class MainWindow;
class Model;
class AtlasMapWidget;

namespace Eris {
  class Entity;
}

namespace Gtk {
  class OptionMenu;
  class TreeStore;
  template <class T> class TreeModelColumn;
  class TreeModelColumnRecord;
  class TreeView;
  class TreeSelection;
}

class EntityTree : public OptionBox
{
  private:
    Gtk::OptionMenu * m_modelMenu;

    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_idColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_typeColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

    AtlasMapWidget * m_attributeTree;

    Model * m_currentModel;

    void descendEntityTree(Eris::Entity *, Gtk::TreeModel::Row & row);
  public:
    MainWindow & m_mainWindow;

    explicit EntityTree(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void currentModelChanged(Model *);
    void modelAdded(Model *);

    gint buttonEvent(GdkEventButton*);

};

#endif // EQUATOR_GUI_GTKMM_ENTITY_TREE_H

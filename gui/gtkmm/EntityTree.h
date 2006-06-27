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

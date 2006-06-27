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

#ifndef EQUATOR_APP_INHERITANCEWINDOW_H
#define EQUATOR_APP_INHERITANCEWINDOW_H

#include "gui/gtkmm/OptionBox.h"

#include <gtkmm/treestore.h>

class MainWindow;
class Server;
class AtlasMapWidget;

namespace Eris {
  class TypeInfo;
}

namespace Gtk {
  class OptionMenu;
  class TreeStore;
  template <class T> class TreeModelColumn;
  class TreeModelColumnRecord;
  class TreeView;
  class TreeSelection;
}

class InheritanceWindow : public OptionBox
{
  private:
    Gtk::OptionMenu * m_serverMenu;

    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

    AtlasMapWidget * m_attributeTree;

    Server * m_currentServer;

    void descendTypeTree(Eris::TypeInfo *, Gtk::TreeModel::Row & row);
  public:
    MainWindow & m_mainWindow;

    explicit InheritanceWindow(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void currentServerChanged(Server *);
    void serverAdded(Server *);

    gint buttonEvent(GdkEventButton*);

};

#endif // EQUATOR_APP_INHERITANCEWINDOW_H

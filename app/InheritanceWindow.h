// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_INHERITANCEWINDOW_H
#define EQUATOR_APP_INHERITANCEWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class AtlasMapWidget;

namespace Gtk {
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
};

class InheritanceWindow : public Gtk::Window
{
  private:
    Gtk::Label * m_connectionLabel;
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_valueColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    AtlasMapWidget * m_attributeTree;

  public:
    MainWindow & m_mainWindow;

    explicit InheritanceWindow(MainWindow &);

    gint delete_event_impl(GdkEventAny*) {
        hide();
        return 1;
    }

    gint buttonEvent(GdkEventButton*);

};

#endif // EQUATOR_APP_INHERITANCEWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_TYPEWINDOW_H
#define EQUATOR_GUI_GTKMM_TYPEWINDOW_H

#include "OptionBox.h"

class Server;

namespace Eris {
   class TypeInfo;
};

namespace Gtk {
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
   class Menu;
};

class TypeTree : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_objTypeColumn;
    Gtk::TreeModelColumn<Eris::TypeInfo *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Gtk::Menu * m_popupMenu;

    Server & m_server;

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void insertType(Eris::TypeInfo * const ti);

  public:
    explicit TypeTree(Server &);

    void populate();
};

#endif // EQUATOR_GUI_GTKMM_TYPEWINDOW_H

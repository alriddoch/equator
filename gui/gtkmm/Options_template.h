// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_OPTIONS_H
#define EQUATOR_GUI_GTKMM_OPTIONS_H

#include "OptionBox.h"

#include <gtkmm/treeview.h>

namespace Gtk {
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
   class Menu;
};

class Options : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<bool> * m_selectColumn;
    Gtk::TreeModelColumn<void *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Gtk::Menu * m_popupMenu;

    ObjectLayer & m_cal3dStore;

  public:
    explicit TypeTree(Server &);

    void populate();
};

#endif // EQUATOR_GUI_GTKMM_OPTIONS_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_CAL3D_STORE_OPTIONS_H
#define EQUATOR_GUI_GTKMM_CAL3D_STORE_OPTIONS_H

#include "OptionBox.h"

#include <gtkmm/treeview.h>

class Cal3dStore;

namespace Gtk {
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
   class Menu;
};

class Cal3dStoreOptions : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<bool> * m_selectColumn;
    Gtk::TreeModelColumn<void *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

    Gtk::TreeModel::Row animationRow;
    Gtk::TreeModel::Row actionRow;
    Gtk::TreeModel::Row meshRow;
    Gtk::TreeModel::Row materialRow;

    Gtk::Menu * m_popupMenu;

    Cal3dStore & m_cal3dStore;

    void skeletonLoaded(const std::string &);
    void animationLoaded(const std::string &, int);
    void actionLoaded(const std::string &, int);
    void meshLoaded(const std::string &, int);
    void materialLoaded(const std::string &, int);
  public:
    explicit Cal3dStoreOptions(Cal3dStore &);

    void populate();
};

#endif // EQUATOR_GUI_GTKMM_CAL3D_STORE_OPTIONS_H

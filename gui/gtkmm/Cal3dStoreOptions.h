// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

typedef enum comp_type { COMP_NONE, COMP_MESH, COMP_MATERIAL, COMP_SKELETON,
                         COMP_ACTION, COMP_ANIMATION, } ComponentType;

class Cal3dStoreOptions : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<bool> * m_selectColumn;
    Gtk::TreeModelColumn<int> * m_idColumn;
    Gtk::TreeModelColumn<ComponentType> * m_componentColumn;
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
    void enableToggled(const Glib::ustring& path_string);
  public:
    explicit Cal3dStoreOptions(Cal3dStore &);

    void populate();
};

#endif // EQUATOR_GUI_GTKMM_CAL3D_STORE_OPTIONS_H

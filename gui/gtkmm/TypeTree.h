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

#ifndef EQUATOR_GUI_GTKMM_TYPETREE_H
#define EQUATOR_GUI_GTKMM_TYPETREE_H

#include "OptionBox.h"

class ImportTypesWizard;
class Server;

namespace Eris {
   class TypeInfo;
};

namespace Gtk {
   class Action;
   class ActionGroup;
   class Menu;
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeRow;
   class TreeRowReference;
   class TreeView;
   class TreeSelection;
   class UIManager;
};

class TypeTree : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Eris::TypeInfo *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Gtk::Menu * m_popupMenu;
    Glib::RefPtr< Gtk::UIManager > m_UIManager;
    Glib::RefPtr< Gtk::ActionGroup > m_actions;
    Glib::RefPtr< Gtk::Action > m_actionImport;

    Server & m_server;
    ImportTypesWizard * m_pImportTypesWizard;

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }
    bool buttonPressEvent(GdkEventButton * pEvent);
    void importPressed();
    void importTypesWizardResponse(int iResponse);
    void insertType(Eris::TypeInfo * pType, Eris::TypeInfo * pParent);

  public:
    explicit TypeTree(Server &);

    void populate();
private:
    std::map< Eris::TypeInfo *, Gtk::TreeRowReference > m_Types;
};

#endif // EQUATOR_GUI_GTKMM_TYPETREE_H

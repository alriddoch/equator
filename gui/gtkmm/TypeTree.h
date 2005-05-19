// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_TYPETREE_H
#define EQUATOR_GUI_GTKMM_TYPETREE_H

#include "OptionBox.h"

#include <Atlas/Objects/RootOperation.h>

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
    void insertType(Eris::TypeInfo * const, Gtk::TreeRow);
    void typeInstalled(const Atlas::Objects::Operation::RootOperation & Operation);

  public:
    explicit TypeTree(Server &);

    void populate();
};

#endif // EQUATOR_GUI_GTKMM_TYPETREE_H

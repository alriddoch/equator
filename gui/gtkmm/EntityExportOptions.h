// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_ENTITY_EXPORT_OPTIONS_H
#define EQUATOR_GUI_GTKMM_ENTITY_EXPORT_OPTIONS_H

#include <gtkmm/dialog.h>

namespace Eris {
  class TypeInfo;
}

namespace Gtk {
  class CheckButton;
  class Entry;
}

class EntityExportOptions : public Gtk::Dialog
{
  private:
    
  public:
    typedef enum export_target { EXPORT_ALL, EXPORT_VISIBLE, EXPORT_SELECTION, EXPORT_ALL_SELECTED } ExportTarget;

    Gtk::Button * m_ok;
    Gtk::CheckButton * m_charCheck; 
    Gtk::CheckButton * m_appendCheck; 
    Gtk::Entry * m_idSuffix;
    Gtk::CheckButton * m_setRootCheck; 
    Gtk::Entry * m_rootId;
    ExportTarget m_target;
    Eris::TypeInfo * m_charType;

    EntityExportOptions();

    void setExportTarget(ExportTarget et);
    void response(int);
};

#endif // EQUATOR_GUI_GTKMM_ENTITY_EXPORT_OPTIONS_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef EQUATOR_GUI_GTKMM_ENTITY_IMPORT_OPTIONS_H
#define EQUATOR_GUI_GTKMM_ENTITY_IMPORT_OPTIONS_H

#include <gtkmm/dialog.h>

class EntityImportOptions : public Gtk::Dialog
{
  private:
    
  public:
    typedef enum import_target { IMPORT_TOPLEVEL, IMPORT_SELECTION } ImportTarget;

    Gtk::Button * m_ok;
    ImportTarget m_target;

    EntityImportOptions();

    void setImportTarget(ImportTarget it);
    void response(int);
};

#endif // EQUATOR_GUI_GTKMM_ENTITY_IMPORT_OPTIONS_H

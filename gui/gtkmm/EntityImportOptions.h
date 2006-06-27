// Equator Online RPG World Building Tool
// Copyright (C) 2000-2004 Alistair Riddoch
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

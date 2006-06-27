// Equator Online RPG World Building Tool
// Copyright (C) 2000-2003 Alistair Riddoch
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

#ifndef EQUATOR_APP_ATLASMAPWIDGET_H
#define EQUATOR_APP_ATLASMAPWIDGET_H

#include <Atlas/Message/Element.h>

#include <gtkmm/treeview.h>

namespace Gtk {
   class TreeStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
};

class AtlasMapWidget : public Gtk::TreeView
{
  private:
    Glib::RefPtr<Gtk::TreeStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_valueColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Atlas::Message::MapType m_contents;

    void add(/* Gtk::CTree_Helpers::RowList,*/ const std::string&,
             const Atlas::Message::Element &);
    void update();
  public:
    AtlasMapWidget(/* const Gtk::SArray &, */
                   const Atlas::Message::MapType &);

    void setContents(const Atlas::Message::MapType &);
    void setAttr(const std::string &, const Atlas::Message::Element &);

};

#endif // EQUATOR_APP_ATLASMAPWIDGET_H

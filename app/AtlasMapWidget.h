// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_ATLASMAPWIDGET_H
#define EQUATOR_APP_ATLASMAPWIDGET_H

#include <Atlas/Message/Object.h>
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
    Atlas::Message::Object::MapType m_contents;

    void add(/* Gtk::CTree_Helpers::RowList,*/ const std::string&,
             const Atlas::Message::Object &);
    void update();
  public:
    AtlasMapWidget(/* const Gtk::SArray &, */
                   const Atlas::Message::Object::MapType &);

    void setContents(const Atlas::Message::Object::MapType &);
    void setAttr(const std::string &, const Atlas::Message::Object &);

};

#endif // EQUATOR_APP_ATLASMAPWIDGET_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "AtlasMapWidget.h"

#include <gtkmm/frame.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treestore.h>

#include <sstream>

using Atlas::Message::Element;

std::ostream & operator<<(std::ostream& s, const Atlas::Message::Element& v)
{
    switch(v.getType()) {
        case Element::TYPE_INT:
            s << v.asInt();
            break;
        case Element::TYPE_FLOAT:
            s << v.asFloat();
            break;
        case Element::TYPE_STRING:
            s << v.asString();
            break;
        default:
            s << "INVALID";
    }
    return s;
}

void AtlasMapWidget::add(/* Gtk::CTree_Helpers::RowList rowList, */
                         const std::string & name,
                         const Element & o)
{
#warning Fix the AtlasMapWidget class FIXME
#if 0
    std::vector<const char*> item(2);
    item[0] = name.c_str();
    if (!o.isList() && !o.isMap()) {
        std::stringstream data;
        data << o;
        item[1] = data.str().c_str();
        rowList.push_back(Gtk::CTree_Helpers::Element(item));
    } else {
        item[1] = "";
        rowList.push_back(Gtk::CTree_Helpers::Element(item));
        Gtk::CTree_Helpers::RowList::iterator i = --rowList.end();
        if (o.isList()) {
            const Element::ListType & olist = o.asList();
            Element::ListType::const_iterator I = olist.begin();
            for (;I != olist.end(); I++) {
                add(i->subtree(), "", *I);
            }
        } else if (o.isMap()) {
            const Element::MapType & omap = o.asMap();
            Element::MapType::const_iterator I = omap.begin();
            for (;I != omap.end(); I++) {
                add(i->subtree(), I->first, I->second);
            }
        }
    }
#endif
}

void AtlasMapWidget::update()
{
    // clear();
    Element::MapType::const_iterator I = m_contents.begin();
    for (;I != m_contents.end(); I++) {
        add(/* rows(),*/ I->first, I->second);
    }
}

AtlasMapWidget::AtlasMapWidget(/* const Gtk::SArray & t, */
                               const Atlas::Message::Element::MapType & m) :
                               m_contents(m)
{
    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_valueColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_valueColumn);

    m_treeModel = Gtk::TreeStore::create(*m_columns);

    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_nameColumn] = Glib::ustring("test name");
    row[*m_valueColumn] = Glib::ustring("test value");

    set_model( m_treeModel );

    if (m_contents.size() != 0) {
        update();
    }
}

void AtlasMapWidget::setContents(const Atlas::Message::Element::MapType & m)
{
    m_contents = m;
    update();
}

void AtlasMapWidget::setAttr(const std::string & n, const Atlas::Message::Element & o)
{
    m_contents[n] = o;
    // Probably don't need to do a full update, but its quicker for now.
    update();
}

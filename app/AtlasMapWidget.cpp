// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "AtlasMapWidget.h"
#include <sstream>

#include <gtk--/frame.h>

typedef Atlas::Message::Object AObject;

std::ostream & operator<<(std::ostream& s, const Atlas::Message::Object& v)
{
    switch(v.GetType()) {
        case AObject::TYPE_INT:
            s << v.AsInt();
            break;
        case AObject::TYPE_FLOAT:
            s << v.AsFloat();
            break;
        case AObject::TYPE_STRING:
            s << v.AsString();
            break;
        default:
            s << "INVALID";
    }
    return s;
}

void AtlasMapWidget::add(Gtk::CTree_Helpers::RowList rowList,
                         const std::string & name,
                         const AObject & o)
{
    std::vector<const char*> item(2);
    item[0] = name.c_str();
    if (!o.IsList() && !o.IsMap()) {
        std::stringstream data;
        data << o;
        item[1] = data.str().c_str();
        rowList.push_back(Gtk::CTree_Helpers::Element(item));
    } else {
        item[1] = "";
        rowList.push_back(Gtk::CTree_Helpers::Element(item));
        Gtk::CTree_Helpers::RowList::iterator i = --rowList.end();
        if (o.IsList()) {
            const AObject::ListType & olist = o.AsList();
            AObject::ListType::const_iterator I = olist.begin();
            for (;I != olist.end(); I++) {
                add(i->subtree(), "", *I);
            }
        } else if (o.IsMap()) {
            const AObject::MapType & omap = o.AsMap();
            AObject::MapType::const_iterator I = omap.begin();
            for (;I != omap.end(); I++) {
                add(i->subtree(), I->first, I->second);
            }
        }
    }

}

void AtlasMapWidget::update()
{
    clear();
    AObject::MapType::const_iterator I = m_contents.begin();
    for (;I != m_contents.end(); I++) {
        add(rows(), I->first, I->second);
    }
}

AtlasMapWidget::AtlasMapWidget(const Gtk::SArray & t,
                               const Atlas::Message::Object::MapType & m) :
                               Gtk::CTree(t), m_contents(m)
{
    if (m_contents.size() != 0) {
        update();
    }
}

void AtlasMapWidget::setContents(const Atlas::Message::Object::MapType & m)
{
    m_contents = m;
    update();
}

void AtlasMapWidget::setAttr(const std::string & n, const Atlas::Message::Object & o)
{
    m_contents[n] = o;
    // Probably don't need to do a full update, but its quicker for now.
    update();
}

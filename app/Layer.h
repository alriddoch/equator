// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LAYER_H
#define EQUATOR_APP_LAYER_H

#include <string>
#include <sigc++/object.h>

class GlView;

namespace Gtk {
  class FileSelection;
}

class Layer : public SigC::Object {
  protected:
    Layer(GlView & w, const std::string & n, const std::string & t) :
          m_window(w), m_name(n), m_type(t), m_visible(true) { }
    virtual ~Layer() { }

    GlView & m_window;
    std::string m_name;
    std::string m_type;
    bool m_visible;
  public:
    virtual void importFile() = 0;
    virtual void draw() = 0;
    virtual void select(int x, int y) = 0;
    virtual void select(int x, int y, int fx, int fy) = 0;

    const std::string & getType() const {
        return m_type;
    }

    const std::string & getName() const {
        return m_name;
    }

    bool isVisible() const {
        return m_visible;
    }
};

#endif // EQUATOR_APP_LAYER_H

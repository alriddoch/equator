// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LAYER_H
#define EQUATOR_APP_LAYER_H

#include <string>
#include <sigc++/object.h>

class GlView;

class Layer : public SigC::Object {
  protected:
    Layer(const GlView & w, const std::string & n, const std::string & t) :
          m_window(w), m_name(n), m_type(t) { }
    virtual ~Layer() { }

    const GlView & m_window;
    std::string m_name;
    std::string m_type;
  public:
    virtual void draw() = 0;

    const std::string & getType() const {
        return m_type;
    }

    const std::string & getName() const {
        return m_name;
    }

    bool isVisible() const {
        // FIXME This is not always true
        return true;
    }
};

#endif // EQUATOR_APP_LAYER_H

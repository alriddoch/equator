// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef EQUATOR_APP_LAYER_H
#define EQUATOR_APP_LAYER_H

#include "common/Vector3D.h"
#include "types.h"

#include <sigc++/object.h>

#include <string>

class GlView;
class Model;

class Layer : public SigC::Object {
  protected:
    Layer(Model & m, const std::string & n, const std::string & t) :
          m_model(m), m_name(n), m_type(t), m_visible(true),
          m_xoff(0.0f), m_yoff(0.0f), m_zoff(0.0f) { }
    virtual ~Layer();

    Model & m_model;
    std::string m_name;
    std::string m_type;
    bool m_visible;
    float m_xoff, m_yoff, m_zoff;
  public:
    virtual void options() = 0;
    virtual void importFile() = 0;
    virtual void exportFile() = 0;
    virtual void selectInvert() = 0;
    virtual void selectAll() = 0;
    virtual void selectNone() = 0;

    virtual void draw(GlView & view) = 0;
    virtual bool animate(GlView & view) = 0;
    virtual void select(GlView & view, int x, int y) = 0;
    virtual void select(GlView & view, int x, int y, int fx, int fy) = 0;
    virtual void pushSelection() = 0;
    virtual void popSelection() = 0;
    virtual void dragStart(GlView & view, int x, int y) = 0;
    virtual void dragUpdate(GlView & view, const WFMath::Vector<3> &) = 0;
    virtual void dragEnd(GlView & view, const WFMath::Vector<3> &) = 0;
    virtual void insert(const PosType &) = 0;
    virtual void align(Alignment) = 0;

    const std::string & getType() const {
        return m_type;
    }

    const std::string & getName() const {
        return m_name;
    }

    bool isVisible() const {
        return m_visible;
    }

    bool toggleVisible() {
        return (m_visible = !m_visible);
    }
};

#endif // EQUATOR_APP_LAYER_H

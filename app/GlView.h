// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_GLVIEW_H
#define EQUATOR_APP_GLVIEW_H

#include <gtkgl--/glarea.h>
#include <gtk--/menu.h>

#include <list>

class Layer;
class ViewWindow;

class GlView : public Gtk::GLArea {
  private:
    Gtk::Menu * m_popup;
    std::list<Layer *> m_layers;
    float m_scale;
    int m_currentLayer;

    enum view { PLAN, ISO, PERSP } m_projection;

    void setPlan();
    void setIsometric();
    void setPerspective();
    void setScale(float s);
    void zoomIn();
    void zoomOut();

    void initgl();
    void setupgl();
    void drawgl();

    virtual void realize_impl();
    // virtual gint motion_notify_event_impl(GdkEventMotion*); 
    virtual gint button_press_event_impl(GdkEventButton*); 
    virtual gint button_release_event_impl(GdkEventButton*); 
    virtual gint expose_event_impl(GdkEventExpose* expose);
    virtual gint configure_event_impl(GdkEventConfigure *event);
  public:
    ViewWindow & m_viewwindow;

    explicit GlView(ViewWindow&);

    float getScale() const {
        return m_scale;
    }

    const std::list<Layer *> & getLayers() const {
        return m_layers;
    }

    const int getCurrentLayer() const {
        return m_currentLayer;
    }

    const std::string details() const;
    void addLayer(Layer *);
};

#endif // EQUATOR_APP_GLVIEW_H

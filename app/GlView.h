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
    Layer * m_currentLayer;
    float m_xoff, m_yoff, m_zoff;
    int clickx,clicky;
    double dragDepth, dragx, dragy, dragz;
    int mousex,mousey;

    enum view { PLAN, ISO, PERSP } m_projection;

    enum drag { NONE, SELECT, MOVE, PAN } m_dragType;

    void setPlan();
    void setIsometric();
    void setPerspective();
    void setScale(float s);
    void zoomIn();
    void zoomOut();

    void initgl();
    void setupgl();
    void origin();
    void drawgl();

    void importFile();

    void clickOn(int x, int y);
    void clickOff(int x, int y);
    void midClickOn(int x, int y);
    void midClickOff(int x, int y);

    void worldPoint(int x, int y, double & z,
                    double * wx, double * wy, double * wz);

    virtual void realize_impl();
    virtual gint motion_notify_event_impl(GdkEventMotion*); 
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

    const Layer * getCurrentLayer() const {
        return m_currentLayer;
    }

    void setCurrentLayer(Layer * l) {
        m_currentLayer = l;
    }

    const std::string details() const;
    void addLayer(Layer *);

    void raiseCurrentLayer();
    void lowerCurrentLayer();

    void setPickProjection();

    const float getZ(int x, int y) const;
};

#endif // EQUATOR_APP_GLVIEW_H

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
class MainWindow;
class Model;

class GlView : public Gtk::GLArea {
  public:
    typedef enum view { ORTHO, PERSP } projection_t;
    typedef enum drag { NONE, SELECT, MOVE, PAN } drag_t;
    typedef enum render { LINE, SOLID, SHADED, TEXTURE, SHADETEXT } rmode_t;
  private:
    Gtk::Menu * m_popup;
    const int m_viewNo;
    float m_scale;
    float m_xoff, m_yoff, m_zoff;
    float m_declination;           // From vertically down
    float m_rotation;              // From due north
    float m_cursX, m_cursY, m_cursZ;
    int clickx,clicky;
    double dragDepth, dragx, dragy, dragz;
    int mousex,mousey;

    projection_t m_projection;
    drag_t m_dragType;
    rmode_t m_renderMode;

    void setOrthographic();
    void setPerspective();

    void setScale(float s);
    void setFace(float d, float r);
    void zoomIn();
    void zoomOut();

    void initgl();
    void setupgl();
    void origin();
    void face();
    void cursor();
    void drawgl();
    void redraw();
    gint animate();

    void clickOn(int x, int y);
    void clickOff(int x, int y);
    void midClickOn(int x, int y);
    void midClickOff(int x, int y);

    void worldPoint(int x, int y, double & z,
                    double * wx, double * wy, double * wz);
    void screenPoint(double x, double y, double z,
                     int & sx, int & sy, double & sz);

    virtual void realize_impl();
    virtual gint motion_notify_event_impl(GdkEventMotion*); 
    virtual gint button_press_event_impl(GdkEventButton*); 
    virtual gint button_release_event_impl(GdkEventButton*); 
    virtual gint expose_event_impl(GdkEventExpose* expose);
    virtual gint configure_event_impl(GdkEventConfigure *event);
  public:
    MainWindow & m_mainWindow;
    ViewWindow & m_viewWindow;
    Model & m_model;

    explicit GlView(MainWindow &, ViewWindow&, Model&);

    float getScale() const {
        return m_scale;
    }

    enum render getRenderMode() {
        return m_renderMode;
    }

    void setRenderMode(enum render m) {
        m_renderMode = m;
    }

    const std::string details() const;

    void setPickProjection();

    const float getZ(int x, int y) const;
};

#endif // EQUATOR_APP_GLVIEW_H

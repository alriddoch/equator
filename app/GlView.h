// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_GLVIEW_H
#define EQUATOR_APP_GLVIEW_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/menu.h>

#include <gtkglmm.h>

#include <list>

class Layer;
class ViewWindow;
class MainWindow;
class Model;

class GlView : public Gtk::DrawingArea {
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
    int clickx,clicky;
    double dragDepth, dragx, dragy, dragz;
    int mousex,mousey;
    unsigned int m_antTexture;
    float m_animCount;

    static bool extensionsInitialised;

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
    gint animate();
    void mouseEffects();
    bool make_current();
    void swap_buffers();

    void clickOn(int x, int y);
    void clickOff(int x, int y);
    void midClickOn(int x, int y);
    void midClickOff(int x, int y);

    void worldPoint(int x, int y, double & z,
                    double * wx, double * wy, double * wz);
    void screenPoint(double x, double y, double z,
                     int & sx, int & sy, double & sz);

    void realize();
    bool motionNotifyEvent(GdkEventMotion*); 
    bool buttonPressEvent(GdkEventButton*); 
    bool buttonReleaseEvent(GdkEventButton*); 
    bool exposeEvent(GdkEventExpose* expose);
    bool configureEvent(GdkEventConfigure *event);

    static void initExtensions();
  public:
    MainWindow & m_mainWindow;
    ViewWindow & m_viewWindow;
    Model & m_model;

    explicit GlView(MainWindow &, ViewWindow&, Model&);

    float getScale() const {
        return m_scale;
    }

    float getXoff() const {
        return m_xoff;
    }

    void setXoff(float x) {
        m_xoff = x;
    }

    float getYoff() const {
        return m_yoff;
    }

    void setYoff(float y) {
        m_yoff = y;
    }

    float getZoff() const {
        return m_zoff;
    }

    void setZoff(float z) {
        m_zoff = z;
    }

    enum render getRenderMode() const {
        return m_renderMode;
    }

    unsigned int getAnts() const {
        return m_antTexture;
    }

    float getAnimCount() const {
        return m_animCount;
    }

    void setRenderMode(enum render m) {
        m_renderMode = m;
    }

    const std::string details() const;

    void setPickProjection();
    void getViewOffset(float & h, float & v, float & d);
    void setViewOffset(float h, float v, float d);
    float getViewSize();
    void redraw();

    const float getZ(int x, int y) const;

    SigC::Signal0<void> viewChanged;
};

#endif // EQUATOR_APP_GLVIEW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_GLVIEW_H
#define EQUATOR_APP_GLVIEW_H

#include "visual/GL.h"

#include <gtkmm/drawingarea.h>
#include <gtkmm/adjustment.h>

#include <gtkglmm.h>

#include <map>
#include <set>

class Layer;
class ViewWindow;
class MainWindow;
class Model;
class Renderer;

class GlView : public Gtk::DrawingArea {
  public:
    typedef enum view { ORTHO, PERSP } projection_t;
    typedef enum drag { NONE, SELECT, MOVE, PAN, ORBIT, ZOOM } drag_t;
    typedef enum render { LINE, SOLID, SHADED, TEXTURE, SHADETEXT, DEFAULT } rmode_t;
  private:
    bool m_redrawRequired;
    bool m_animationRequired;
    GLubyte * m_frameStore;
    GLfloat * m_depthStore;
    int m_frameStoreWidth;
    int m_frameStoreHeight;

    bool redraw();
  public:
    const int m_viewNo;
    Gtk::Adjustment & m_scaleAdj;
    Gtk::Adjustment & m_xAdj;
    Gtk::Adjustment & m_yAdj;
    Gtk::Adjustment & m_zAdj;
    Gtk::Adjustment & m_declAdj;
    Gtk::Adjustment & m_rotaAdj;
    // float m_scale;
    // float m_xoff, m_yoff, m_zoff;
    // float m_declination;           // From vertically down
    // float m_rotation;              // From due north
    int clickx,clicky;
    double dragDepth, dragx, dragy, dragz;
    int mousex,mousey;
    unsigned int m_antTexture;
    float m_animCount;

    static bool extensionsInitialised;

    projection_t m_projection;
    drag_t m_dragType;
    rmode_t m_renderMode;

    std::map<std::string, rmode_t> m_renderModes;
    std::set<std::string> m_hiddenLayers;

    void setOrthographic();
    void setPerspective();

    float getScale() const;
    void setLogScale(float s);
    void setScale(float s);

    void setFace(float d, float r);
    void zoomIn();
    void zoomOut();
    void setHome();

    void showCameraControl();

    void initgl();
    void setupgl();
    void origin();
    void face();
    void cursor();
    void drawgl();
    bool animate();
    void mouseEffects();
    bool make_current();
    void swap_buffers();

    void clickOn(int x, int y);
    void clickOff(int x, int y);
    void midClickOn(int x, int y);
    void midClickOff(int x, int y);

    void worldPoint(int x, int y, double & z,
                    double * wx, double * wy, double * wz);
    void screenPoint(float x, float y, float z,
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
    Renderer & m_renderer;

    explicit GlView(MainWindow &, ViewWindow&, Model&);

    Gtk::Adjustment & getScaleAdjustment() {
        return m_scaleAdj;
    }

    Gtk::Adjustment & getXAdjustment() {
        return m_xAdj;
    }

    Gtk::Adjustment & getYAdjustment() {
        return m_yAdj;
    }

    Gtk::Adjustment & getZAdjustment() {
        return m_zAdj;
    }

    Gtk::Adjustment & getRotationAdjustment() {
        return m_rotaAdj;
    }

    Gtk::Adjustment & getDeclinationAdjustment() {
        return m_declAdj;
    }

    float getLogScale() const {
        return m_scaleAdj.get_value();
    }

    float getXoff() const {
        return m_xAdj.get_value();
    }

    void setXoff(float x) {
        m_xAdj.set_value(x);
    }

    float getYoff() const {
        return m_yAdj.get_value();
    }

    void setYoff(float y) {
        m_yAdj.set_value(y);
    }

    float getZoff() const {
        return m_zAdj.get_value();
    }

    void setZoff(float z) {
        m_zAdj.set_value(z);
    }

    float getDeclination() const {
        return m_declAdj.get_value();
    }

    void setDeclination(float d) {
        m_declAdj.set_value(d);
    }

    float getRotation() const {
        return m_rotaAdj.get_value();
    }

    void setRotation(float r) {
        m_rotaAdj.set_value(r);
    }

    rmode_t getDefaultRenderMode() const {
        return m_renderMode;
    }

    unsigned int getAnts() const {
        return m_antTexture;
    }

    float getAnimCount() const {
        return m_animCount;
    }

    void setRenderMode(rmode_t m) {
        m_renderMode = m;
    }

    const std::set<std::string> & getHiddenLayers() const {
        return m_hiddenLayers;
    }
    
    void hideLayer(const std::string & l) {
        m_hiddenLayers.insert(l);
    }

    void showLayer(const std::string & l) {
        m_hiddenLayers.erase(l);
    }

    const std::string details() const;

    void scheduleRedraw();
    void startAnimation();
    void setPickProjection(int nx, int ny, int fx, int fy);
    void getViewOffset(float & h, float & v, float & d);
    void setViewOffset(float h, float v, float d);
    float getViewSize();
    rmode_t getRenderMode(const std::string & layer) const;
    void setLayerRenderMode(rmode_t m);
    void enableAnts();
    void disableAnts();

    const float getZ(int x, int y) const;
};

#endif // EQUATOR_APP_GLVIEW_H

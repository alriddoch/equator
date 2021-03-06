// Equator Online RPG World Building Tool
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "GL.h"

#include "GlView.h"
#include "Model.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "Layer.h"

#include "visual/Renderer.h"

#include "common/debug.h"

#include "gui/gtkmm/CameraControl.h"
#include "gui/gtkmm/DockWindow.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include <GL/glu.h>

#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/main.h>

#include <sigc++/functors/mem_fun.h>

#include <iostream>
#include <sstream>

#include <inttypes.h>

#include <cassert>
#include <cmath>

static const bool pretty = true;

static const float Deg2Rad = M_PI / 180.0f;

static const bool debug_flag = false;

float deg2Rad(float d)
{
    return (d * Deg2Rad);
}

static const float cursorLines[] = { 0.0f, 0.2f, 0.0f,
                                     0.0f, 0.6f, 0.0f,
                                     0.0f, -0.2f, 0.0f,
                                     0.0f, -0.6f, 0.0f,
                                     0.2f, 0.0f, 0.0f,
                                     0.6f, 0.0f, 0.0f,
                                     -0.2f, 0.0f, 0.0f,
                                     -0.6f, 0.0f, 0.0f };

static const float cursorCircle[] = { 0.0f, 0.4f, 0.0f,
                                      0.2f, 0.3464f, 0.0f,
                                      0.3464f, 0.2f, 0.0f,
                                      0.4f, 0.0f, 0.0f,
                                      0.3464f, -0.2f, 0.0f,
                                      0.2f, -0.3464f, 0.0f,
                                      0.0f, -0.4f, 0.0f,
                                      -0.2f, -0.3464f, 0.0f,
                                      -0.3464f, -0.2f, 0.0f,
                                      -0.4f, 0.0f, 0.0f,
                                      -0.3464f, 0.2f, 0.0f,
                                      -0.2f, 0.3464f, 0.0f };

GlView::GlView(MainWindow&mw,ViewWindow&vw, Model&m) :
           m_redrawRequired(false),
           m_animationRequired(false),
           m_frameStore(0),
           m_viewNo(m.getViewNo()),
           m_scaleAdj(*manage( new Gtk::Adjustment(0.0, -16, 16) )),
           m_xAdj(*manage( new Gtk::Adjustment(0., -500., 500.) )),
           m_yAdj(*manage( new Gtk::Adjustment(0., -500., 500.) )),
           m_zAdj(*manage( new Gtk::Adjustment(0., -500., 500.) )),
           m_declAdj(*manage( new Gtk::Adjustment(60., 0., 360.) )),
           m_rotaAdj(*manage( new Gtk::Adjustment(45., 0., 360.) )),
           clickx(0), clicky(0),
           dragx(0.0), dragy(0.0), dragz(0.0),
           m_animCount(0.0),
           m_dragType(NONE),
           m_mainWindow(mw),
           m_viewWindow(vw),
           m_model(m),
           m_renderer(* new Renderer)
{
    m_scaleAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_xAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_yAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_zAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_declAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_rotaAdj.signal_value_changed().connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));

    m_projection = GlView::ORTHO; // KEEPME
    m_renderMode = GlView::SOLID; // KEEPME

    Glib::RefPtr<Gdk::GL::Config> glconfig = Gdk::GL::Config::create(
                                             Gdk::GL::MODE_RGB |
                                             Gdk::GL::MODE_DEPTH |
                                             // Gdk::GL::MODE_ACCUM |
                                             Gdk::GL::MODE_DOUBLE);
#if defined(SIGC_MAJOR_VERSION) && SIGC_MAJOR_VERSION < 2
    if (glconfig.is_null()) {
        std::cerr << "*** Cannot find the double-buffered visual.\n"
                  << "*** Trying single-buffered visual.\n";

        // Try single-buffered visual
        glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                         Gdk::GL::MODE_DEPTH);
        if (glconfig.is_null()) {
            std::cerr << "*** Cannot find any OpenGL-capable visual.\n";

        }
    }

    if (!glconfig.is_null()) {
        Gtk::GL::Widget::set_gl_capability(*this, glconfig);
    }
#else
    // FIXME Be a bit more thorough, like above.

    if (mw.m_glContext) {
        std::cout << "Context exists" << std::endl << std::flush;
        Gtk::GL::widget_set_gl_capability(*this, glconfig, mw.m_glContext);
    } else {
        std::cout << "No Context" << std::endl << std::flush;
        Gtk::GL::widget_set_gl_capability(*this, glconfig);
        // mw.m_glContext = Gtk::GL::widget_create_gl_context(*this);
    }
#endif

    m.updated.connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));

    set_events(Gdk::POINTER_MOTION_MASK|
               Gdk::EXPOSURE_MASK|
               Gdk::BUTTON_PRESS_MASK|
               Gdk::BUTTON_RELEASE_MASK);

    mw.modeChanged.connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    mw.toolChanged.connect(sigc::mem_fun(*this, &GlView::scheduleRedraw));
    m_model.cursorMoved.connect(sigc::mem_fun(m_viewWindow,
                                           &ViewWindow::cursorMoved));
    signal_realize().connect(sigc::mem_fun(*this, &GlView::realize));
    // signal_configure_event().connect(sigc::mem_fun(*this, &GlView::configureEvent));
    signal_expose_event().connect(sigc::mem_fun(*this, &GlView::exposeEvent));
    signal_button_press_event().connect(sigc::mem_fun(*this, &GlView::buttonPressEvent));
    signal_button_release_event().connect(sigc::mem_fun(*this, &GlView::buttonReleaseEvent));
    signal_motion_notify_event().connect(sigc::mem_fun(*this, &GlView::motionNotifyEvent));

    scheduleRedraw();
    startAnimation();
}

void GlView::setOrthographic()
{
    m_projection = ORTHO;
    m_viewWindow.setTitle();
}

void GlView::setPerspective()
{
    m_projection = PERSP;
    m_viewWindow.setTitle();
}

float GlView::getScale() const
{
    // Representation of the scale inside the adjustment is
    // log2 of the scale factor, to give the user a sane scale
    return pow(2, m_scaleAdj.get_value());
}

void GlView::setLogScale(GLfloat s)
{
    m_scaleAdj.set_value(s);
    m_viewWindow.setTitle();
}

void GlView::setScale(GLfloat s)
{
    // Representation of the scale inside the adjustment is
    // log2 of the scale factor, to give the user a sane scale
    setLogScale(log2(s));
}

void GlView::setFace(GLfloat d, GLfloat r)
{
    m_declAdj.set_value(d);
    m_rotaAdj.set_value(r);
}

void GlView::zoomIn()
{
    float new_scale = getScale() * 2.f;
    if (new_scale > 16.f) {
        new_scale = 16.f;
    }
    if (getScale() != new_scale) {
        setScale(new_scale);
    }
}

void GlView::zoomOut()
{
    float new_scale = getScale() / 2;
    if (new_scale < 0.0078125f) {
        new_scale = 0.0078125f;
    }
    if (getScale() != new_scale) {
        setScale(new_scale);
    }
}

void GlView::setHome()
{
    setXoff(0);
    setYoff(0);
    setZoff(0);
}

void GlView::showCameraControl()
{
    CameraControl * c = new CameraControl(*this);
    DockWindow * dw = new DockWindow(*c);

    dw->show_all();
}

bool GlView::extensionsInitialised = 0;

void GlView::initExtensions()
{
    extensionsInitialised = true;
}

void GlView::initgl()
{
    if (!make_current()) {
        return;
    }

    if (!extensionsInitialised) {
        initExtensions();
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDepthFunc(GL_LEQUAL);

    uint8_t ants[] = { 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0x0, 0x0,
                       0x0, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0xff,
                       0xff, 0xff, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &m_antTexture);
    glBindTexture(GL_TEXTURE_1D, m_antTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 8, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, ants);
    if (glGetError() != 0) {
        std::cerr << "Failed to create ants" << std::endl << std::flush;
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // glEnable(GL_ACCUM);

}

void GlView::setupgl()
{
    glViewport(0, 0, (GLint)(get_width()), (GLint)(get_height()));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (m_projection == GlView::PERSP) {
        GLfloat w = (GLfloat) get_width() / (GLfloat) get_height();
        gluPerspective(45.f, w, .1f, 60.f);
    } else {
        float xsize = get_width() / 40.0f / 2.0f;
        float ysize = get_height() / 40.0f / 2.0f;
        glOrtho(-xsize, xsize, -ysize, ysize, -100.0f, 100.0f);
    }
}

void GlView::origin()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (m_projection == GlView::PERSP) {
        glTranslatef(0.0f, 0.0f, -10.0f);
    }
    glRotatef(-getDeclination(), 1.0f, 0.0f, 0.0f);
    glRotatef(getRotation(), 0.0f, 0.0f, 1.0f);
    float scale = getScale();
    glScalef(scale, scale, scale);
    glTranslatef(getXoff(), getYoff(), getZoff());
}

void GlView::face()
{
    float scale = getScale();
    glScalef(1.0f/scale, 1.0f/scale, 1.0f/scale);
    glRotatef(-getRotation(), 0.0f, 0.0f, 1.0f);
    glRotatef(getDeclination(), 1.0f, 0.0f, 0.0f);
}

void GlView::cursor()
{
    glVertexPointer(3, GL_FLOAT, 0, cursorLines);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, 8);
    glVertexPointer(3, GL_FLOAT, 0, cursorCircle);
    glColor3f(1.0f, 0.2f, 0.2f);
    glDrawArrays(GL_LINE_LOOP, 0, 12);
}

void GlView::drawgl()
{
    origin();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();
    float cx, cy, cz;
    m_model.getCursor(cx, cy, cz);
    glTranslatef(cx, cy, cz);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, m_antTexture);
    const GLfloat vertices[] = { 0.f, 0.f, 0.f, 0.f, 0.f, -cz };
    const GLfloat texcoords[] = { 0.f, cz };
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(1, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_1D);
    face();
    cursor();
    glPopMatrix();

    const std::list<Layer *> & layers = m_model.getLayers();
    std::list<Layer *>::const_iterator I;
    for(I = layers.begin(); I != layers.end(); I++) {
        if ((*I)->isVisible()) {
            (*I)->draw(*this);
        }
    }
    glFlush();

    GLenum er;
    if ((er = glGetError()) != 0) {
        std::cerr << "Reporting OpenGL rendering error: " << gluErrorString(er)
                  << std::endl << std::flush;
    }

    mouseEffects();
}

void GlView::clickOn(int x, int y)
{
    if (m_dragType != GlView::NONE) {
        return;
    }
    switch (m_mainWindow.getTool()) {
        case MainWindow::SELECT:
            if (make_current()) {
                m_model.getCurrentLayer()->select(*this, x, get_height() - y);
            }
            break;
        case MainWindow::AREA:
            clickx = x; clicky = y;
            m_dragType = GlView::SELECT;
            break;
        case MainWindow::MOVE:
            clickx = x; clicky = y;
            m_model.getCurrentLayer()->dragStart(*this, x, get_height() - y);
            dragDepth = -2;
            worldPoint(x, y, dragDepth, &dragx, &dragy, &dragz);
            m_dragType = GlView::MOVE;
            break;
        case MainWindow::DRAW:
            {
            double cursDepth;
            int tx, ty;
            // Get the depth in the current view of the cursor
            float cx, cy, cz;
            m_model.getCursor(cx, cy, cz);
            screenPoint(cx, cy, cz, tx, ty, cursDepth);
            double nx, ny, nz;
            double z = getZ(x, get_height() - y);
            if (z > 0.99) {
                z = cursDepth;
            }
            // Calculate the world coords with the same depth, at the current
            // position of the mouse.
            worldPoint(x, y, z, &nx, &ny, &nz);
            m_model.setCursor(nx, ny, nz);
            }
            break;
        case MainWindow::ROTATE:
        case MainWindow::SCALE:
        default:
            break;
    }
    scheduleRedraw();
}

void GlView::clickOff(int x, int y)
{
    switch (m_dragType) {
        case GlView::SELECT:
            if (make_current()) {
                if ((clickx == x) && (clicky == y)) {
                    m_model.getCurrentLayer()->select(*this, x, get_height() - y);
                } else {
                    m_model.getCurrentLayer()->select(*this, clickx, get_height() - clicky, x, get_height() - y);
                }
            }
            break;
        case GlView::MOVE:
            {
                double tx, ty, tz;
                worldPoint(x, y, dragDepth, &tx, &ty, &tz);
                // Send move thingy to layer
                m_model.getCurrentLayer()->dragEnd(*this, WFMath::Vector<3>(tx - dragx, ty - dragy, tz - dragz));
            }
            break;
        default:
            break;
    }
    clickx = 0; clicky = 0;
    m_dragType = GlView::NONE;
    scheduleRedraw();
}

void GlView::midClickOn(int x, int y)
{
    if (m_dragType != GlView::NONE) {
        return;
    }
    dragDepth = -2;
    worldPoint(x, y, dragDepth, &dragx, &dragy, &dragz);
    clickx = x;
    clicky = y;
    switch (m_mainWindow.getNavMode()) {
        case MainWindow::PAN:
            m_dragType = GlView::PAN;
            break;
        case MainWindow::ORBIT:
            m_dragType = GlView::ORBIT;
            break;
        case MainWindow::ZOOM:
            m_dragType = GlView::ZOOM;
            break;
        default:
            break;
    }
#if 0
    if (m_mainWindow.getTool() == MainWindow::DRAW) {
        float cx, cy, cz;
        m_model.getCursor(cx, cy, cz);
        m_model.getCurrentLayer()->insert(PosType(cx, cy, cz));
    }
#endif
}

void GlView::midClickOff(int x, int y)
{
    double tx, ty, tz;
    int dx = x - clickx,
        dy = y - clicky;
    switch (m_dragType) {
        case GlView::PAN:
            worldPoint(x, y, dragDepth, &tx, &ty, &tz);
            setXoff(getXoff() + (tx - dragx) );
            setYoff(getYoff() + (ty - dragy) );
            setZoff(getZoff() + (tz - dragz) );
            break;
        case GlView::ORBIT:
            {
                float rot = getRotation() + (dx * 360.f) / get_width();
                float dec = getDeclination() - (dy * 180.f) / get_height();
                while (rot > 360.f) { rot -= 360.f; }
                while (rot < 0.f) { rot += 360.f; }
                while (dec > 360.f) { dec -= 360.f; }
                while (dec < 0.f) { dec += 360.f; }
                setRotation(rot);
                setDeclination(dec);
            }
            break;
        case GlView::ZOOM:
            {
                float sc = getLogScale() + (dy * 4.f) / get_height();
                if (sc > 16.f) { sc = 16.f; }
                if (sc < -16.f) { sc = -16.f; }
                setLogScale(sc);
            }
            break;
        default:
            break;
    }
    clickx = 0;
    clicky = 0;
    m_dragType = GlView::NONE;
}

void GlView::worldPoint(int x, int y, double &z,
                        double * wx, double * wy, double * wz)
{
    if (!make_current()) {
        return;
    }
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    // float z = 0.5;
    if (z < -1) {
        z = getZ(x, get_height() - y);
    }

    setupgl();
    origin();

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    gluUnProject(x, get_height() - y, z, mvmatrix, projmatrix, viewport, wx, wy, wz);
}

void GlView::screenPoint(float x, float y, float z,
                         int & sx, int & sy, double & sz)
{
    if (!make_current()) {
        return;
    }

    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    // float z = 0.5;
    if (z < -1) {
        z = getZ(::lrintf(x), ::lrintf(get_height() - y));
    }

    setupgl();
    origin();

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    double tx, ty, tz;
    gluProject(x, y, z, mvmatrix, projmatrix, viewport, &tx, &ty, &tz);
    sx = ::lrint(tx);
    sy = ::lrint(get_height() - ty);
    sz = tz;
}

void GlView::realize()
{
    initgl();
    if (!m_mainWindow.m_glContext) {
        m_mainWindow.m_glContext = Gtk::GL::widget_get_gl_context(*this);
    }
}

bool GlView::motionNotifyEvent(GdkEventMotion*event)
{
    mousex = lrint(event->x); mousey = lrint(event->y);
    if (clickx == 0) {
        return true;
    }
    double tx, ty, tz;
    int dx = mousex - clickx,
        dy = mousey - clicky;
    switch (m_dragType) {
        case GlView::NONE:
        case GlView::SELECT:
        case GlView::MOVE:
            startAnimation();
            switch (m_mainWindow.getTool()) {
                case MainWindow::MOVE:
                case MainWindow::ROTATE:
                case MainWindow::SCALE:
                    {
                        // Send move thingy to layer
                        worldPoint(mousex, mousey, dragDepth, &tx, &ty, &tz);
                        m_model.getCurrentLayer()->dragUpdate(*this, WFMath::Vector<3>(tx - dragx, ty - dragy, tz - dragz));
                    }
                    break;
                case MainWindow::SELECT:
                case MainWindow::DRAW:
                case MainWindow::AREA:
                default:
                    break;
            }
            break;
        case GlView::PAN:
            {
                double z = getZ(mousex, get_height() - mousey);
                if (z > 0.99) {
                    z = dragDepth;
                }
                worldPoint(mousex, mousey, z, &tx, &ty, &tz);
                setXoff(getXoff() + (tx - dragx) );
                setYoff(getYoff() + (ty - dragy) );
                setZoff(getZoff() + (tz - dragz) );
            }
            break;
        case GlView::ORBIT:
            {
                float rot = getRotation() + (dx * 360.f) / get_width();
                float dec = getDeclination() - (dy * 180.f) / get_height();
                while (rot > 360.f) { rot -= 360.f; }
                while (rot < 0.f) { rot += 360.f; }
                while (dec > 360.f) { dec -= 360.f; }
                while (dec < 0.f) { dec += 360.f; }
                setRotation(rot);
                setDeclination(dec);
                clickx = mousex;
                clicky = mousey;
            }
            break;
        case GlView::ZOOM:
            {
                float sc = getLogScale() + (dy * 4.f) / get_height();
                if (sc > 16.f) { sc = 16.f; }
                if (sc < -16.f) { sc = -16.f; }
                setLogScale(sc);
                clickx = mousex;
                clicky = mousey;
            }
            break;
        default:
            break;
    }
    return true;
}

void GlView::mouseEffects()
{
    // We may want to be a little smarter about this
    if (clickx == 0) {
        return;
    }
    switch (m_mainWindow.getTool()) {
      case MainWindow::AREA:
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, get_width(), 0, get_height(), -100.0f, 200.0f);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            // glClear( GL_COLOR_BUFFER_BIT);
            glTranslatef(clickx, get_height() - clicky, 100.0f);
            float x = mousex - clickx;
            float y = clicky - mousey;
            std::cout << clickx << ":" << clicky << ":" << mousex << ":" << mousey << " " << x << ":" << y << std::endl << std::flush;
            const GLfloat dvertices[] = { 0.f, 0.f, 0.f,
                                          x, 0.f, 0.f,
                                          x, y, 0.f,
                                          0.f, y, 0.f,
                                          0.f, 0.f, 0.f };

            glVertexPointer(3, GL_FLOAT, 0, dvertices);
            // Set The Blending Function For Translucency
            glBlendFunc(GL_SRC_ALPHA,GL_ONE);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glDrawArrays(GL_LINE_STRIP, 0, 5);
            glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
        }
        break;
      default:
        break;
    }
}

bool GlView::make_current()
{
#if defined(SIGC_MAJOR_VERSION) && SIGC_MAJOR_VERSION < 2
    Glib::RefPtr<Gdk::GL::Context> glcontext =
      Gtk::GL::Widget::get_gl_context(*this);

    Glib::RefPtr<Gdk::GL::Window> glwindow =
      Gtk::GL::Widget::get_gl_window(*this);
#else
    Glib::RefPtr<Gdk::GL::Context> glcontext =
      Gtk::GL::widget_get_gl_context(*this);

    Glib::RefPtr<Gdk::GL::Window> glwindow =
      Gtk::GL::widget_get_gl_window(*this);
#endif

    if (!glwindow->make_current(glcontext)) {
        return false;
    }
    return true;
}

void GlView::swap_buffers()
{

#if defined(SIGC_MAJOR_VERSION) && SIGC_MAJOR_VERSION < 2
    Glib::RefPtr<Gdk::GL::Window> glwindow =
      Gtk::GL::Widget::get_gl_window(*this);
#else
    Glib::RefPtr<Gdk::GL::Window> glwindow =
      Gtk::GL::widget_get_gl_window(*this);
#endif

    if (glwindow->is_double_buffered()) {
        glwindow->swap_buffers();
    }

}

bool GlView::buttonPressEvent(GdkEventButton * event)
{
    debug(std::cout << "BUTTON" << event->button << std::endl << std::flush;);
    switch (event->button) {
        case 1:
            clickOn(::lrint(event->x), ::lrint(event->y));
            break;
        case 2:
            midClickOn(::lrint(event->x), ::lrint(event->y));
            break;
        case 3:
            m_mainWindow.setCurrentModel(&m_model);
            break;
        default:
            break;
    }
    return TRUE;
}

bool GlView::buttonReleaseEvent(GdkEventButton * event)
{
    switch (event->button) {
        case 1:
            clickOff(::lrint(event->x), ::lrint(event->y));
            break;
        case 2:
            midClickOff(::lrint(event->x), ::lrint(event->y));
            break;
        case 3:
        default:
            break;
    }
    return TRUE;
}

bool GlView::exposeEvent(GdkEventExpose * event)
{
    if (event->count > 0) {
        return 0;
    }
    scheduleRedraw();
    return TRUE;
}

bool GlView::configureEvent(GdkEventConfigure*)
{
    if (!make_current()) {
        return TRUE;
    }
    setupgl();
    return TRUE;
}

const std::string GlView::details() const
{
    std::stringstream dets;
    const char * view = (m_projection == ORTHO) ? "Orthographic" : "Perspective";
    dets << "-" << m_model.getModelNo() << "." << m_viewNo << " (" << view << ") " << (int)(getScale() * 100) << "%";
    return dets.str();
}

void GlView::startAnimation()
{
    if (!m_animationRequired) {
        m_animationRequired = true;
        Glib::signal_timeout().connect(sigc::mem_fun(*this, &GlView::animate), 50);
    }
}

bool GlView::animate()
{
    if (!m_animationRequired) {
        return false;
    }
    if (!make_current()) {
        return true;
    }
    m_animationRequired = false;

    m_animCount += 0.02f;
    if (m_animCount > 1.0f) {
        m_animCount = 0.0f;
    }

    // FIXME animate() should not do any rendering. setupgl should go after.
    setupgl();
    origin();

    m_mainWindow.updateTime();

    const std::list<Layer *> & layers = m_model.getLayers();
    std::list<Layer *>::const_iterator I = layers.begin();
    std::list<Layer *>::const_iterator Iend = layers.end();
    for(;I != Iend; I++) {
        if ((*I)->isVisible()) {
            if ((*I)->animate(*this)) {
                m_animationRequired = true;
            }
        }
    }

    drawgl();

    swap_buffers();
    return true;
}

void GlView::scheduleRedraw()
{
    if (!m_redrawRequired) {
        m_redrawRequired = true;
        Glib::signal_idle().connect(sigc::mem_fun(*this, &GlView::redraw));
    }
}

bool GlView::redraw()
{
    if (!m_redrawRequired) {
        return false;
    }
    if (!make_current()) {
        return true;
    }
    m_redrawRequired = false;

    setupgl();
    drawgl();

    swap_buffers();
    return false;
}

void GlView::setPickProjection(int nx, int ny, int fx, int fy)
{
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    int sWidth = abs(fx - nx);
    int sHeight = abs(fy - ny);
    int sXCentre = (fx > nx) ? (nx + sWidth / 2) : (fx + sWidth / 2);
    int sYCentre = (fy > ny) ? (ny + sHeight / 2) : (fy + sHeight / 2);
    std::cout << "PICK: " << sXCentre << ":" << sYCentre << ":"
              << sWidth << ":" << sHeight << std::endl << std::flush;
    gluPickMatrix(sXCentre, sYCentre, sWidth, sHeight, viewport);

    if (m_projection == GlView::PERSP) {
#if 0
        if (get_width()>get_height()) {
            GLfloat w = (GLfloat) get_width() / (GLfloat) get_height();
            glFrustum( -w, w, -1.0f, 1.0f, 0.65f, 60.0f );
        } else {
            GLfloat h = (GLfloat) get_height() / (GLfloat) get_width();
            glFrustum( -1.0f, 1.0f, -h, h, 0.65f, 60.0f );
        }
#else
        GLfloat w = (GLfloat) get_width() / (GLfloat) get_height();
        gluPerspective(45.f, w, .1f, 60.f);
#endif
    } else {
        float xsize = get_width() / 40.0f / 2.0f;
        float ysize = get_height() / 40.0f / 2.0f;
        glOrtho(-xsize, xsize, -ysize, ysize, -100.0f, 100.0f);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glInitNames();
    if (m_projection == GlView::PERSP) {
        glTranslatef(0.0f, 0.0f, -10.0f);
    }
    glRotatef(-getDeclination(), 1.0f, 0.0f, 0.0f);
    glRotatef(getRotation(), 0.0f, 0.0f, 1.0f);
    float scale = getScale();
    glScalef(scale, scale, scale);
    glTranslatef(getXoff(), getYoff(), getZoff());
}

void GlView::getViewOffset(float & h, float & v, float & d)
{
    WFMath::Vector<3> vo(getXoff(), getYoff(), getZoff());
    vo.rotateZ(deg2Rad(getRotation()));
    vo.rotateX(-deg2Rad(getDeclination()));
    // vo.rotateZ(-getRotation());
    // vo.rotateX(m_declination);

    h = vo.x();
    v = vo.y();
    d = vo.z();

    debug(std::cout << "Getting getViewOffset " << h << ":" << v << ":" << d
                    << " " << getXoff() << ":" << getYoff() << ":" << getZoff()
                    << std::endl << std::flush;);
}

void GlView::setViewOffset(float h, float v, float d)
{
    WFMath::Vector<3> vo(h, v, d);
    vo.rotateX(deg2Rad(getDeclination()));
    vo.rotateZ(-deg2Rad(getRotation()));

    setXoff(vo.x());
    setYoff(vo.y());
    setZoff(vo.z());
}

float GlView::getViewSize()
{
    float winsize = std::sqrt((float)(get_width() * get_width() + get_height() * get_height()));

    return (winsize / (40.0f * getScale()) + 1);
}

GlView::rmode_t GlView::getRenderMode(const std::string & layer) const
{
    std::map<std::string,rmode_t>::const_iterator I = m_renderModes.find(layer);
    if (I == m_renderModes.end()) {
        return getDefaultRenderMode();
    } else {
        return I->second;
    }
}

void GlView::setLayerRenderMode(rmode_t m)
{
    Layer * l = m_model.getCurrentLayer();
    if (0 != l) {
        if (GlView::DEFAULT == m) {
            m_renderModes.erase(l->getName());
        } else {
            m_renderModes[l->getName()] = m;
        }
    }
}

void GlView::enableAnts()
{
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    float phase = getAnimCount();
    glTranslatef(phase, phase, phase);
    glMatrixMode(GL_MODELVIEW);

    GLfloat scale = 0.5f * getScale();
    GLfloat sx[] = { scale, scale, scale, 0 };

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, getAnts());
    glColor3f(1.f, 1.f, 1.f);
    glEnable(GL_TEXTURE_GEN_S);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx);
}

void GlView::disableAnts()
{
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_1D);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

const float GlView::getZ(int x, int y) const
{
    float z = 0;
    glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    debug(std::cout << "GOT Z " << z << std::endl << std::flush;);
    return z;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GlView.h"
#include "Model.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "Layer.h"

#include <wfmath/point.h>

#include <GL/glu.h>

#include <gtk--/main.h>
#include <gtk--/menuitem.h>
#include <gtk--/menushell.h>
#include <gtk--/fileselection.h>

#include <iostream>
#include <sstream>

#include <inttypes.h>

int attrlist[] = {
    GDK_GL_RGBA,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_DEPTH_SIZE, 1,
    GDK_GL_ACCUM_RED_SIZE, 1,
    GDK_GL_ACCUM_GREEN_SIZE, 1,
    GDK_GL_ACCUM_BLUE_SIZE, 1,
    GDK_GL_NONE
};

static const bool pretty = true;

static const float Deg2Rad = M_PI / 180.0f;

float deg2Rad(float d)
{
    return (d * Deg2Rad);
}

GlView::GlView(MainWindow&mw,ViewWindow&vw, Model&m) : Gtk::GLArea(attrlist),
                               m_popup(NULL),
                               m_viewNo(m.getViewNo()),
                               m_scale(1.0),
                               // m_currentLayer(NULL),
                               m_xoff(0), m_yoff(0), m_zoff(0),
                               m_declination(60), m_rotation(45),
                               clickx(0), clicky(0),
                               dragx(0.0), dragy(0.0), dragz(0.0),
                               m_animCount(0.0),
                               m_dragType(NONE),
                               m_mainWindow(mw),
                               m_viewWindow(vw),
                               m_model(m)
{
    m.updated.connect(slot(this, &GlView::redraw));

    set_events(GDK_POINTER_MOTION_MASK|
               GDK_EXPOSURE_MASK|
               GDK_BUTTON_PRESS_MASK|
               GDK_BUTTON_RELEASE_MASK);

    m_popup = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& list_popup = m_popup->items();
    list_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());

    Gtk::Menu *menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& file_popup = menu_sub->items();
    file_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Save"));
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Save As..."));
    file_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Import...", SigC::slot(&m_model, &Model::importFile)));
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Export...", SigC::slot(&m_model, &Model::exportFile)));
    file_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Close"));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("File",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& edit_popup = menu_sub->items();
    edit_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Undo"));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Redo"));
    edit_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Cut", Gtk::Menu_Helpers::CTL|'x'));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Copy", Gtk::Menu_Helpers::CTL|'c'));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Paste", Gtk::Menu_Helpers::CTL|'v'));
    edit_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Delete", Gtk::Menu_Helpers::CTL|'d'));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Edit",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& select_popup = menu_sub->items();
    select_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("Invert"));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("All"));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("None"));
    select_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("Push", '>', slot(&m_model, &Model::pushSelection)));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("Pop", '<', slot(&m_model, &Model::popSelection)));
    select_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());

    Gtk::Menu * menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& align_popup = menu_sub_sub->items();
    align_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    align_popup.push_back(Gtk::Menu_Helpers::MenuElem("to heightfield", SigC::bind<Alignment>(slot(&m_model, &Model::alignSelection), ALIGN_HEIGHT)));
    align_popup.push_back(Gtk::Menu_Helpers::MenuElem("to grid", SigC::bind<Alignment>(slot(&m_model, &Model::alignSelection), ALIGN_GRID)));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("Align", *menu_sub_sub));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Select",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& view_popup = menu_sub->items();
    view_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom In", '=', slot(this, &GlView::zoomIn)));
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom Out", '-', slot(this, &GlView::zoomOut)));
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& zoom_popup = menu_sub_sub->items();
    zoom_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("16:1", SigC::bind<float>(slot(this, &GlView::setScale), 16)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("8:1", SigC::bind<float>(slot(this, &GlView::setScale), 8)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("4:1", SigC::bind<float>(slot(this, &GlView::setScale), 4)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("2:1", SigC::bind<float>(slot(this, &GlView::setScale), 2)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:1", '1', SigC::bind<float>(slot(this, &GlView::setScale), 1)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:2", SigC::bind<float>(slot(this, &GlView::setScale), 0.5f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:4", SigC::bind<float>(slot(this, &GlView::setScale), 0.25f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:8", SigC::bind<float>(slot(this, &GlView::setScale), 0.125f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:16", SigC::bind<float>(slot(this, &GlView::setScale), 0.0625f)));
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom", *menu_sub_sub));
    view_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    Gtk::RadioMenuItem::Group projection_group;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(projection_group,
                         "Orthographic", slot(this, &GlView::setOrthographic)));
    static_cast<Gtk::RadioMenuItem*>(view_popup.back())->set_active();
    m_projection = GlView::ORTHO;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(projection_group,
                         "Perspective", slot(this, &GlView::setPerspective)));
    view_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    Gtk::RadioMenuItem::Group render_group;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(render_group, "Line", SigC::bind<enum render>(slot(this, &GlView::setRenderMode),LINE)));
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(render_group, "Solid", SigC::bind<enum render>(slot(this, &GlView::setRenderMode),SOLID)));
    static_cast<Gtk::RadioMenuItem*>(view_popup.back())->set_active();
    m_renderMode = GlView::SOLID;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(render_group, "Shaded", SigC::bind<enum render>(slot(this, &GlView::setRenderMode),SHADED)));
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(render_group, "Textured", SigC::bind<enum render>(slot(this, &GlView::setRenderMode),TEXTURE)));
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(render_group, "Lit", SigC::bind<enum render>(slot(this, &GlView::setRenderMode),SHADETEXT)));
    view_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& face_popup = menu_sub_sub->items();
    face_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("Isometric", "KP_5", SigC::bind<float, float>(slot(this, &GlView::setFace), 60, 45)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("North", "KP_1", SigC::bind<float, float>(slot(this, &GlView::setFace), 90, 0)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("South", SigC::bind<float, float>(slot(this, &GlView::setFace), 90, 180)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("West", "KP_3", SigC::bind<float, float>(slot(this, &GlView::setFace), 90, -90)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("East", SigC::bind<float, float>(slot(this, &GlView::setFace), 90, 90)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("Down", "KP_7", SigC::bind<float, float>(slot(this, &GlView::setFace), 0, 0)));
    face_popup.push_back(Gtk::Menu_Helpers::MenuElem("Up", SigC::bind<float, float>(slot(this, &GlView::setFace), 180, 0)));
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Face..", *menu_sub_sub));
    view_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("New View", SigC::bind<Model*>(slot(&m_mainWindow, &MainWindow::newView),&m_model)));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("View",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& layer_popup = menu_sub->items();
    layer_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    layer_popup.push_back(Gtk::Menu_Helpers::MenuElem("Layers...", slot(&m_mainWindow, &MainWindow::openLayers)));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Layers",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& tools_popup = menu_sub->items();
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& generic_tools_popup = menu_sub_sub->items();
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Select single", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::SELECT)));
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Select area", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::AREA)));
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Insert", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::DRAW)));
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Rotate", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::ROTATE)));
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Scale", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::SCALE)));
    generic_tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Translate", bind(slot(&m_mainWindow,&MainWindow::toolSelect),MainWindow::MOVE)));
    tools_popup.push_back(Gtk::Menu_Helpers::MenuElem("Generic", *menu_sub_sub));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Tools",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& filters_popup = menu_sub->items();
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& entity_filters_popup = menu_sub_sub->items();
    entity_filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("Align to heightmap"));
    filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("Entity", *menu_sub_sub));
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& tile_filters_popup = menu_sub_sub->items();
    tile_filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("Do tily thing"));
    filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("Tile", *menu_sub_sub));
    menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& height_filters_popup = menu_sub_sub->items();
    height_filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("Match edges"));
    filters_popup.push_back(Gtk::Menu_Helpers::MenuElem("HeightMap", *menu_sub_sub));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Filters",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& net_popup = menu_sub->items();
    net_popup.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    net_popup.push_back(Gtk::Menu_Helpers::MenuElem("Connect.."));
    net_popup.push_back(Gtk::Menu_Helpers::MenuElem("Disconnect..."));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Net",*menu_sub));

    // list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Float 3"));

    // Gtk::Main::timeout.connect(slot(this, &GlView::animate), 100);

    m_popup->accelerate(m_viewWindow);

    m_model.cursorMoved.connect(SigC::slot(&m_viewWindow,
                                           &ViewWindow::cursorMoved));
}

void GlView::setOrthographic()
{
    m_projection = ORTHO;
    redraw();
    m_viewWindow.setTitle();
}

void GlView::setPerspective()
{
    m_projection = PERSP;
    redraw();
    m_viewWindow.setTitle();
}

void GlView::setScale(GLfloat s)
{
    m_scale = s;
    redraw();
    m_viewWindow.setTitle();
    viewChanged.emit();
}

void GlView::setFace(GLfloat d, GLfloat r)
{
    m_declination = d;
    m_rotation = r;
    redraw();
    viewChanged.emit();
}

void GlView::zoomIn()
{
    float new_scale = m_scale * 2.0f;
    if (new_scale > 16.0f) {
        new_scale = 16.0f;
    }
    if (m_scale != new_scale) {
        setScale(new_scale);
    }
}

void GlView::zoomOut()
{
    float new_scale = m_scale / 2;
    if (new_scale < 0.0078125f) {
        new_scale = 0.0078125f;
    }
    if (m_scale != new_scale) {
        setScale(new_scale);
    }
}

void GlView::initgl()
{
    if (make_current()) {
        setupgl();
    }

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

    glEnable(GL_ACCUM);
}

void GlView::setupgl()
{
    if (make_current()) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, (GLint)(width()), (GLint)(height()));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (m_projection == GlView::PERSP) {
            if (width()>height()) {
                GLfloat w = (GLfloat) width() / (GLfloat) height();
                glFrustum( -w, w, -1.0f, 1.0f, 1.0f, 60.0f );
            } else {
                GLfloat h = (GLfloat) height() / (GLfloat) width();
                glFrustum( -1.0f, 1.0f, -h, h, 1.0f, 60.0f );
            }
        } else {
            float xsize = width() / 40.0f / 2.0f;
            float ysize = height() / 40.0f / 2.0f;
            glOrtho(-xsize, xsize, -ysize, ysize, -0.0f, 1000.0f);
        }
    }
}

void GlView::origin()
{
    // Not safe to call when make_current() has not been called
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(-m_declination, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotation, 0.0f, 0.0f, 1.0f);
    glScalef(m_scale, m_scale, m_scale);
    glTranslatef(m_xoff, m_yoff, m_zoff);
}

void GlView::face()
{
    glScalef(1.0f/m_scale, 1.0f/m_scale, 1.0f/m_scale);
    glRotatef(-m_rotation, 0.0f, 0.0f, 1.0f);
    glRotatef(m_declination, 1.0f, 0.0f, 0.0f);
}

void GlView::cursor()
{
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.2f, 0.0f);
    glVertex3f(0.0f, 0.6f, 0.0f);
    glVertex3f(0.0f, -0.2f, 0.0f);
    glVertex3f(0.0f, -0.6f, 0.0f);
    glVertex3f(0.2f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.0f, 0.0f);
    glVertex3f(-0.2f, 0.0f, 0.0f);
    glVertex3f(-0.6f, 0.0f, 0.0f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.2f, 0.2f);
    glVertex3f(0.0f, 0.4f, 0.0f);
    glVertex3f(0.2f, 0.3464f, 0.0f);
    glVertex3f(0.3464f, 0.2f, 0.0f);
    glVertex3f(0.4f, 0.0f, 0.0f);
    glVertex3f(0.3464f, -0.2f, 0.0f);
    glVertex3f(0.2f, -0.3464f, 0.0f);
    glVertex3f(0.0f, -0.4f, 0.0f);
    glVertex3f(-0.2f, -0.3464f, 0.0f);
    glVertex3f(-0.3464f, -0.2f, 0.0f);
    glVertex3f(-0.4f, 0.0f, 0.0f);
    glVertex3f(-0.3464f, 0.2f, 0.0f);
    glVertex3f(-0.2f, 0.3464f, 0.0f);
    glEnd();
}

void GlView::drawgl()
{
    if (make_current()) {
        origin();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glPushMatrix();
        float cx, cy, cz;
        m_model.getCursor(cx, cy, cz);
        glTranslatef(cx, cy, cz);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, m_antTexture);
        glBegin(GL_LINES);
        glTexCoord1f(0.0f); glVertex3f(0.0f,0.0f,0.0f);
        glTexCoord1f(cz); glVertex3f(0.0f,0.0f,-cz);
        glEnd();
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
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width(), 0, height(), -100.0f, 200.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   // Set The Blending Function For Translucency
        if ((clickx != 0) && (m_dragType == GlView::SELECT)) {
            glTranslatef(clickx, height() - clicky, 100.0f);
            float x = mousex - clickx;
            float y = clicky - mousey;
            std::cout << clickx << ":" << clicky << ":" << mousex << ":" << mousey << " " << x << ":" << y << std::endl << std::flush;
            if (pretty) {
                glEnable(GL_BLEND);
                glDisable(GL_DEPTH_TEST);
                glBegin(GL_LINES);
                glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(x, 0.0f, 0.0f);
                glVertex3f(x, 0.0f, 0.0f);
                glVertex3f(x, y, 0.0f);
                glVertex3f(x, y, 0.0f);
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();
                glBegin(GL_QUADS);
                glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(x, 0.0f, 0.0f);
                glVertex3f(x, y, 0.0f);
                glVertex3f(0.0f, y, 0.0f);
                glEnd();
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            } else {
                glBegin(GL_LINES);
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(x, 0.0f, 0.0f);
                glVertex3f(x, 0.0f, 0.0f);
                glVertex3f(x, y, 0.0f);
                glVertex3f(x, y, 0.0f);
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();
            }
        }
    }
    glFlush();
    swap_buffers();
    glAccum(GL_LOAD, 1.0f);
}

gint GlView::animate()
{
    m_animCount += 0.02f;
    if (m_animCount > 1.0f) {
        m_animCount = 0.0f;
    }
    if (make_current()) {
        setupgl();
        origin();
        // glClear( GL_COLOR_BUFFER_BIT );
        glAccum(GL_RETURN, 1.0f);
        // cursor();
        const std::list<Layer *> & layers = m_model.getLayers();
        std::list<Layer *>::const_iterator I;
        for(I = layers.begin(); I != layers.end(); I++) {
            if ((*I)->isVisible()) {
                (*I)->animate(*this);
            }
        }
        mouseEffects();
        swap_buffers();
    }
    return 1;
}

void GlView::clickOn(int x, int y)
{
    switch (m_mainWindow.getTool()) {
        case MainWindow::SELECT:
            if (make_current()) {
                m_model.getCurrentLayer()->select(*this, mousex, height() - mousey);
            }
            break;
        case MainWindow::AREA:
            clickx = x; clicky = y;
            m_dragType = GlView::SELECT;
            break;
        case MainWindow::MOVE:
            //clickx = x; clicky = y;
            m_model.getCurrentLayer()->dragStart(*this, x, height() - y);
            dragDepth = -2;
            worldPoint(x, y, dragDepth, &dragx, &dragy, &dragz);
            m_dragType = GlView::MOVE;
            break;
        case MainWindow::DRAW:
            double cursDepth;
            int tx, ty;
            // Get the depth in the current view of the cursor
            float cx, cy, cz;
            m_model.getCursor(cx, cy, cz);
            screenPoint(cx, cy, cz, tx, ty, cursDepth);
            double nx, ny, nz;
            // Calculate the world coords with the same depth, at the current
            // position of the mouse.
            worldPoint(x, y, cursDepth, &nx, &ny, &nz);
            m_model.setCursor(nx, ny, nz);
            break;
        case MainWindow::ROTATE:
        case MainWindow::SCALE:
        default:
            break;
    }
    redraw();
}

void GlView::clickOff(int x, int y)
{
    switch (m_mainWindow.getTool()) {
        case MainWindow::AREA:
            if (make_current()) {
                if ((clickx == mousex) && (clicky == clicky)) {
                    m_model.getCurrentLayer()->select(*this, mousex, height() - mousey);
                } else {
                    m_model.getCurrentLayer()->select(*this, clickx, height() - clicky, mousex, height() - mousey);
                }
            }
            m_dragType = GlView::NONE;
            break;
        case MainWindow::MOVE:
            {
                double tx, ty, tz;
                worldPoint(x, y, dragDepth, &tx, &ty, &tz);
                // Send move thingy to layer
                m_model.getCurrentLayer()->dragEnd(*this, tx - dragx, ty - dragy, tz - dragz);
                m_dragType = GlView::NONE;
            }
            break;
        case MainWindow::SELECT:
        case MainWindow::DRAW:
        case MainWindow::ROTATE:
        case MainWindow::SCALE:
            break;
    }
    clickx = 0; clicky = 0;
    redraw();
}

void GlView::midClickOn(int x, int y)
{
    dragDepth = -2;
    worldPoint(x, y, dragDepth, &dragx, &dragy, &dragz);
    m_dragType = GlView::MOVE;
    if (m_mainWindow.getTool() == MainWindow::DRAW) {
        float cx, cy, cz;
        m_model.getCursor(cx, cy, cz);
        m_model.getCurrentLayer()->insert(WFMath::Point<3>(cx, cy, cz));
    }
}

void GlView::midClickOff(int x, int y)
{
    double tx, ty, tz;
    worldPoint(x, y, dragDepth, &tx, &ty, &tz);
    m_xoff += (tx - dragx);
    m_yoff += (ty - dragy);
    m_zoff += (tz - dragz);
    m_dragType = GlView::NONE;
    redraw();
}

void GlView::worldPoint(int x, int y, double &z,
                        double * wx, double * wy, double * wz)
{
    if (make_current()) {
        GLint viewport[4];
        GLdouble mvmatrix[16], projmatrix[16];
        // float z = 0.5;
        if (z < -1) {
            z = getZ(x, height() - y);
        }

        setupgl();
        origin();

        glGetIntegerv (GL_VIEWPORT, viewport);
        glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
        glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

        gluUnProject(x, height() - y, z, mvmatrix, projmatrix, viewport, wx, wy, wz);
        std::cout << "[" << x << ":" << y << ":" << z << "]";
        std::cout << "{" << *wx << ":" << *wy << ":" << *wz << "}" << std::endl << std::flush;
    }
}

void GlView::screenPoint(double x, double y, double z,
                         int & sx, int & sy, double & sz)
{
    if (make_current()) {
        GLint viewport[4];
        GLdouble mvmatrix[16], projmatrix[16];
        // float z = 0.5;
        if (z < -1) {
            z = getZ(x, height() - y);
        }

        setupgl();
        origin();

        glGetIntegerv (GL_VIEWPORT, viewport);
        glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
        glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

        double tx, ty, tz;
        gluProject(x, y, z, mvmatrix, projmatrix, viewport, &tx, &ty, &tz);
        sx = tx;
        sy = height() - ty;
        sz = tz;
    }
}

void GlView::realize_impl()
{
    Gtk::GLArea::realize_impl();
    initgl();
}

gint GlView::motion_notify_event_impl(GdkEventMotion*event)
{
    mousex = event->x; mousey = event->y;
    if (clickx != 0) {
        if (make_current()) {
            glAccum(GL_RETURN, 1.0f);
            mouseEffects();
            swap_buffers();
        }
    }
}

void GlView::mouseEffects()
{
    // We may want to be a little smarter about this
    if (clickx != 0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width(), 0, height(), -100.0f, 200.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBlendFunc(GL_SRC_ALPHA,GL_ONE); // Set The Blending Function For Translucency
        // glClear( GL_COLOR_BUFFER_BIT);
        glTranslatef(clickx, height() - clicky, 100.0f);
        float x = mousex - clickx;
        float y = clicky - mousey;
        std::cout << clickx << ":" << clicky << ":" << mousex << ":" << mousey << " " << x << ":" << y << std::endl << std::flush;
        if (pretty) {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBegin(GL_LINES);
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(x, y, 0.0f);
            glVertex3f(x, y, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glEnd();
            glBegin(GL_QUADS);
            glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(x, y, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
            glEnd();
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        } else {
            glBegin(GL_LINES);
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(x, y, 0.0f);
            glVertex3f(x, y, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glEnd();
        }
    }
}

int GlView::button_press_event_impl(GdkEventButton * event)
{
    std::cout << "BUTTON" << event->button << std::endl << std::flush;
    switch (event->button) {
        case 1:
            clickOn(event->x, event->y);
            break;
        case 2:
            midClickOn(event->x, event->y);
            break;
        case 3:
            m_mainWindow.setCurrentModel(&m_model);
            m_popup->popup(event->button,event->time);
            break;
        default:
            break;
    }
    return TRUE;
}

int GlView::button_release_event_impl(GdkEventButton * event)
{
    switch (event->button) {
        case 1:
            clickOff(event->x, event->y);
            break;
        case 2:
            midClickOff(event->x, event->y);
            break;
        case 3:
        default:
            break;
    }
    return TRUE;
}

int GlView::expose_event_impl(GdkEventExpose * event)
{
    if (event->count>0)
        return 0;
    redraw();
    return TRUE;
}

int GlView::configure_event_impl(GdkEventConfigure*)
{
    setupgl();
    return TRUE;
}

const std::string GlView::details() const
{
    std::stringstream dets;
    const char * view = (m_projection == ORTHO) ? "Orthographic" : "Perspective";
    dets << "-" << m_model.getModelNo() << "." << m_viewNo << " (" << view << ") " << (int)(m_scale * 100) << "%";
    return dets.str();
}

void GlView::redraw()
{
    setupgl();
    drawgl();
}

void GlView::setPickProjection()
{
    if (m_projection == GlView::PERSP) {
        if (width()>height()) {
            GLfloat w = (GLfloat) width() / (GLfloat) height();
            glFrustum( -w, w, -1.0f, 1.0f, 0.65f, 60.0f );
        } else {
            GLfloat h = (GLfloat) height() / (GLfloat) width();
            glFrustum( -1.0f, 1.0f, -h, h, 0.65f, 60.0f );
        }
    } else {
        float xsize = width() / 40.0f / 2.0f;
        float ysize = height() / 40.0f / 2.0f;
        glOrtho(-xsize, xsize, -ysize, ysize, -0.0f, 1000.0f);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glInitNames();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(-m_declination, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotation, 0.0f, 0.0f, 1.0f);
    glScalef(m_scale, m_scale, m_scale);
    glTranslatef(m_xoff, m_yoff, m_zoff);
}

void GlView::getViewOffset(float & h, float & v, float & d)
{
    WFMath::Vector<3> vo(m_xoff, m_yoff, m_zoff);
    vo.rotateZ(deg2Rad(m_rotation));
    vo.rotateX(-deg2Rad(m_declination));
    // vo.rotateZ(-m_rotation);
    // vo.rotateX(m_declination);

    h = vo.x();
    v = vo.y();
    d = vo.z();

    std::cout << "Getting getViewOffset " << h << ":" << v << ":" << d
              << " " << m_xoff << ":" << m_yoff << ":" << m_zoff
              << std::endl << std::flush;
}

void GlView::setViewOffset(float h, float v, float d)
{
    WFMath::Vector<3> vo(h, v, d);
    vo.rotateX(deg2Rad(m_declination));
    vo.rotateZ(-deg2Rad(m_rotation));

    m_xoff = vo.x();
    m_yoff = vo.y();
    m_zoff = vo.z();

    redraw();
    std::cout << "Setting setViewOffset " << h << ":" << v << ":" << d
              << " " << m_xoff << ":" << m_yoff << ":" << m_zoff
              << std::endl << std::flush;
}

float GlView::getViewSize()
{
    float winsize = std::sqrt((float)(width() * width() + height() * height()));

    return (winsize / (40.0f * getScale()) + 1);
}

const float GlView::getZ(int x, int y) const
{
    float z = 0;
    glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    std::cout << "GOT Z " << z << std::endl << std::flush;
    return z;
}

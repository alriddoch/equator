// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GlView.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "Holo.h"
#include "LayerWindow.h"

#include "sstream.h"

#include <GL/gl.h>

#include <gtk--/menuitem.h>
#include <gtk--/menushell.h>

#include <iostream>

GlView::GlView(ViewWindow&w) : m_popup(NULL), m_scale(1.0), m_currentLayer(0),
                               m_viewwindow(w)
{
    set_events(GDK_EXPOSURE_MASK|
               GDK_BUTTON_PRESS_MASK|
               GDK_BUTTON_RELEASE_MASK);

    m_layers.push_front( new Holo(*this) );

    m_popup = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& list_popup = m_popup->items();

    Gtk::Menu *menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& file_popup = menu_sub->items();
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Save"));
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Save As..."));
    file_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_popup.push_back(Gtk::Menu_Helpers::MenuElem("Close"));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("File",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& edit_popup = menu_sub->items();
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Undo"));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Redo"));
    edit_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Cut"));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Copy"));
    edit_popup.push_back(Gtk::Menu_Helpers::MenuElem("Paste"));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Edit",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& select_popup = menu_sub->items();
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("Invert"));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("All"));
    select_popup.push_back(Gtk::Menu_Helpers::MenuElem("None"));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Select",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& view_popup = menu_sub->items();
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom In", '=', slot(this, &GlView::zoomIn)));
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom Out", '-', slot(this, &GlView::zoomOut)));
    Gtk::Menu * menu_sub_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& zoom_popup = menu_sub_sub->items();
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("16:1", SigC::bind<float>(slot(this, &GlView::setScale), 16)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("8:1", SigC::bind<float>(slot(this, &GlView::setScale), 8)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("4:1", SigC::bind<float>(slot(this, &GlView::setScale), 4)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("2:1", SigC::bind<float>(slot(this, &GlView::setScale), 2)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("_1:1", '1', SigC::bind<float>(slot(this, &GlView::setScale), 1)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:2", SigC::bind<float>(slot(this, &GlView::setScale), 0.5f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:4", SigC::bind<float>(slot(this, &GlView::setScale), 0.25f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:8", SigC::bind<float>(slot(this, &GlView::setScale), 0.125f)));
    zoom_popup.push_back(Gtk::Menu_Helpers::MenuElem("1:16", SigC::bind<float>(slot(this, &GlView::setScale), 0.0625f)));
    view_popup.push_back(Gtk::Menu_Helpers::MenuElem("Zoom", *menu_sub_sub));
    view_popup.push_back(Gtk::Menu_Helpers::SeparatorElem());
    Gtk::RadioMenuItem::Group projection_group;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(projection_group,
                         "Plan", slot(this, &GlView::setPlan)));
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(projection_group,
                         "Isometric", slot(this, &GlView::setIsometric)));
    static_cast<Gtk::RadioMenuItem*>(view_popup.back())->set_active();
    m_projection = GlView::ISO;
    view_popup.push_back(Gtk::Menu_Helpers::RadioMenuElem(projection_group,
                         "Perspective", slot(this, &GlView::setPerspective)));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("View",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& layer_popup = menu_sub->items();
    layer_popup.push_back(Gtk::Menu_Helpers::MenuElem("Layers...", SigC::bind<GlView*>(slot(&m_viewwindow.m_mainwindow, &MainWindow::open_layers),this)));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Layers",*menu_sub));

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& net_popup = menu_sub->items();
    net_popup.push_back(Gtk::Menu_Helpers::MenuElem("Connect.."));
    net_popup.push_back(Gtk::Menu_Helpers::MenuElem("Disconnect..."));

    list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Net",*menu_sub));

    // list_popup.push_back(Gtk::Menu_Helpers::MenuElem("Float 3"));

    m_popup->accelerate(m_viewwindow);
}

void GlView::setPlan()
{
    m_projection = PLAN;
    if (make_current()) {
        setupgl();
        drawgl();
    }
    m_viewwindow.setTitle();
}

void GlView::setIsometric()
{
    m_projection = ISO;
    if (make_current()) {
        setupgl();
        drawgl();
    }
    m_viewwindow.setTitle();
}

void GlView::setPerspective()
{
    m_projection = PERSP;
    if (make_current()) {
        setupgl();
        drawgl();
    }
    m_viewwindow.setTitle();
}

void GlView::setScale(GLfloat s)
{
    m_scale = s;
    if (make_current()) {
        drawgl();
    }
    m_viewwindow.setTitle();
}

void GlView::zoomIn()
{
    float new_scale = m_scale * 2.0f;
    if (new_scale > 16.0f) {
        new_scale = 16.0f;
    }
    setScale(new_scale);
}

void GlView::zoomOut()
{
    float new_scale = m_scale / 2;
    if (new_scale < 0.0625f) {
        new_scale = 0.0625f;
    }
    setScale(new_scale);
}

void GlView::initgl()
{
    if (make_current()) {
        setupgl();
    }
}

void GlView::setupgl()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, (GLint)(width()), (GLint)(height()));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

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
        glOrtho(-xsize, xsize, -ysize, ysize, -100000.0f, 100000.0f);
    }
}

void GlView::drawgl()
{
    if (make_current()) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        if (m_projection == GlView::PLAN) {
            glTranslatef(0.0f, 0.0f, -10.0f);
            glScalef(m_scale, m_scale, m_scale);
        } else {
            glTranslatef(0.0f, 0.0f, -10.0f);
            glRotatef(-60.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
            glScalef(m_scale, m_scale, m_scale);
        }
        std::list<Layer *>::const_iterator I;
        for(I = m_layers.begin(); I != m_layers.end(); I++) {
            (*I)->draw();
        }
    }
    swap_buffers();
}

void GlView::realize_impl()
{
    Gtk::GLArea::realize_impl();
    initgl();
}

int GlView::button_press_event_impl(GdkEventButton * event)
{
    m_popup->popup(event->button,event->time);
    return TRUE;
}

int GlView::button_release_event_impl(GdkEventButton*)
{
    return TRUE;
}

int GlView::expose_event_impl(GdkEventExpose * event)
{
    if (event->count>0) return 0;
    drawgl();
    return TRUE;
}

int GlView::configure_event_impl(GdkEventConfigure*)
{
    if (make_current()) {
        setupgl();
    }
    return TRUE;
}

const std::string GlView::details() const
{
    std::stringstream dets;
    const char * view = (m_projection == PLAN) ? "Plan" : (m_projection == ISO) ? "Isometric" : "Perspective";
    dets << " (" << view << ") " << (int)(m_scale * 100) << "%";
    return dets.str();
}

void GlView::addLayer(Layer * layer)
{
    std::list<Layer *>::iterator I = m_layers.begin();
    for (int i = m_currentLayer; i > -1 && I != m_layers.end(); I++, i--) {}
    m_layers.insert(I, layer);
    m_viewwindow.m_mainwindow.getLayerWindow()->setView(this);
}

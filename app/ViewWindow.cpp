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

#include "ViewWindow.h"

#include "GlView.h"
#include "Model.h"
#include "MainWindow.h"

#include "common/debug.h"

#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/table.h>
#include <gtkmm/ruler.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/stock.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>

#include <iostream>
#include <sstream>
#include <cmath>

using Gtk::Menu_Helpers::SeparatorElem;
using Gtk::Menu_Helpers::MenuElem;
using Gtk::Menu_Helpers::RadioMenuElem;
using Gtk::Menu_Helpers::TearoffMenuElem;
using Gtk::Menu_Helpers::StockMenuElem;
using Gtk::Menu_Helpers::MenuList;

#if defined(SIGC_MAJOR_VERSION) && SIGC_MAJOR_VERSION < 2
using Gtk::Menu_Helpers::AccelKey;
#else
using Gtk::AccelKey;
#endif

static const bool debug_flag = false;

ViewWindow::ViewWindow(MainWindow & w, Model & m) : m_glarea(0),
                                                    m_scrollLock(false)
{
    MainWindow & m_mainWindow = w;
    Model & m_model = m;

    m.nameChanged.connect(sigc::mem_fun(*this, &ViewWindow::setTitle));
    // destroy.connect(sigc::mem_fun(this, &ViewWindow::destroy_handler));

    // This needs to be done before the GlView is created
    m_cursorCoords = manage( new Gtk::Statusbar() );
    m_cursorContext = m_cursorCoords->get_context_id("Cursor coordinates");
    m_cursorCoords->push(" xxx,xxx,xxx ", m_cursorContext);

    m_viewCoords = manage( new Gtk::Statusbar() );
    m_viewContext = m_viewCoords->get_context_id("View coordinates");
    m_viewCoords->push(" xxx,xxx,xxx ", m_cursorContext);

    m_glarea = manage( new GlView(w, *this, m) );
    m_glarea->set_size_request(600,400);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    Gtk::MenuBar * menubar = manage( new Gtk::MenuBar() );
    vbox->pack_start(*menubar, Gtk::PACK_SHRINK);

    Gtk::Menu *menu_sub = manage( new Gtk::Menu() );
    MenuList& file_menu = menu_sub->items();
    file_menu.push_back(TearoffMenuElem());
    file_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::SAVE)));
    file_menu.back().set_sensitive(false);
    file_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::SAVE_AS)));
    file_menu.back().set_sensitive(false);
    file_menu.push_back(SeparatorElem());
    file_menu.push_back(MenuElem("Import...", sigc::mem_fun(m_model, &Model::importFile)));
    file_menu.push_back(MenuElem("Export...", sigc::mem_fun(m_model, &Model::exportFile)));
    file_menu.push_back(SeparatorElem());
    file_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::CLOSE)));
    file_menu.back().set_sensitive(false);

    Gtk::MenuItem * menuitem = manage( new Gtk::MenuItem("File") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& edit_menu = menu_sub->items();
    edit_menu.push_back(TearoffMenuElem());
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::UNDO)));
    edit_menu.back().set_sensitive(false);
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::REDO)));
    edit_menu.back().set_sensitive(false);
    edit_menu.push_back(SeparatorElem());
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::CUT)));
    edit_menu.back().set_sensitive(false);
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::COPY)));
    edit_menu.back().set_sensitive(false);
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::PASTE)));
    edit_menu.back().set_sensitive(false);
    edit_menu.push_back(SeparatorElem());
    edit_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::DELETE)));
    edit_menu.back().set_sensitive(false);

    menuitem = manage( new Gtk::MenuItem("Edit") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& select_menu = menu_sub->items();
    select_menu.push_back(TearoffMenuElem());
    select_menu.push_back(MenuElem("Invert", sigc::mem_fun(m_model, &Model::selectInvert)));
    select_menu.push_back(MenuElem("All", sigc::mem_fun(m_model, &Model::selectAll)));
    select_menu.push_back(MenuElem("None", sigc::mem_fun(m_model, &Model::selectNone)));
    select_menu.push_back(SeparatorElem());
    select_menu.push_back(MenuElem("Push", AccelKey('>', Gdk::ModifierType(0)), sigc::mem_fun(m_model, &Model::pushSelection)));
    select_menu.push_back(MenuElem("Pop", AccelKey('<', Gdk::ModifierType(0)), sigc::mem_fun(m_model, &Model::popSelection)));
    select_menu.push_back(SeparatorElem());

    Gtk::Menu * menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& align_menu = menu_sub_sub->items();
    align_menu.push_back(TearoffMenuElem());
    align_menu.push_back(MenuElem("to parent", sigc::bind<Alignment>(sigc::mem_fun(m_model, &Model::alignSelection), ALIGN_PARENT)));
    align_menu.push_back(MenuElem("to grid", sigc::bind<Alignment>(sigc::mem_fun(m_model, &Model::alignSelection), ALIGN_GRID)));
    select_menu.push_back(MenuElem("Align", *menu_sub_sub));

    menuitem = manage( new Gtk::MenuItem("Select") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& view_menu = menu_sub->items();
    view_menu.push_back(TearoffMenuElem());
    view_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::ZOOM_IN), AccelKey('=', Gdk::ModifierType(0)), sigc::mem_fun(*m_glarea, &GlView::zoomIn)));
    view_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::ZOOM_OUT), AccelKey('-', Gdk::ModifierType(0)), sigc::mem_fun(*m_glarea, &GlView::zoomOut)));
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& zoom_menu = menu_sub_sub->items();
    zoom_menu.push_back(TearoffMenuElem());
    zoom_menu.push_back(MenuElem("16:1", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 16)));
    zoom_menu.push_back(MenuElem("8:1", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 8)));
    zoom_menu.push_back(MenuElem("4:1", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 4)));
    zoom_menu.push_back(MenuElem("2:1", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 2)));
    zoom_menu.push_back(MenuElem("1:1", AccelKey('1', Gdk::ModifierType(0)), sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 1)));
    zoom_menu.push_back(MenuElem("1:2", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 0.5f)));
    zoom_menu.push_back(MenuElem("1:4", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 0.25f)));
    zoom_menu.push_back(MenuElem("1:8", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 0.125f)));
    zoom_menu.push_back(MenuElem("1:16", sigc::bind<float>(sigc::mem_fun(*m_glarea, &GlView::setScale), 0.0625f)));
    view_menu.push_back(MenuElem("Zoom", *menu_sub_sub));
    view_menu.push_back(SeparatorElem());
    Gtk::RadioMenuItem::Group projection_group;
    view_menu.push_back(RadioMenuElem(projection_group,
                         "Orthographic", sigc::mem_fun(*m_glarea, &GlView::setOrthographic)));
    static_cast<Gtk::RadioMenuItem*>(&view_menu.back())->set_active();
    view_menu.push_back(RadioMenuElem(projection_group,
                         "Perspective", sigc::mem_fun(*m_glarea, &GlView::setPerspective)));
    view_menu.push_back(SeparatorElem());
    Gtk::RadioMenuItem::Group render_group;
    view_menu.push_back(RadioMenuElem(render_group, "Line", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setRenderMode),GlView::LINE)));
    view_menu.push_back(RadioMenuElem(render_group, "Solid", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setRenderMode),GlView::SOLID)));
    static_cast<Gtk::RadioMenuItem*>(&view_menu.back())->set_active();
    // m_renderMode = GlView::SOLID;
    view_menu.push_back(RadioMenuElem(render_group, "Shaded", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setRenderMode),GlView::SHADED)));
    view_menu.push_back(RadioMenuElem(render_group, "Textured", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setRenderMode),GlView::TEXTURE)));
    view_menu.push_back(RadioMenuElem(render_group, "Lit", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setRenderMode),GlView::SHADETEXT)));
    view_menu.push_back(SeparatorElem());
    view_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::HOME), sigc::mem_fun(*m_glarea, &GlView::setHome)));
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& face_menu = menu_sub_sub->items();
    face_menu.push_back(TearoffMenuElem());
    face_menu.push_back(MenuElem("Isometric", AccelKey("KP_5"), sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 60, 45)));
    face_menu.push_back(MenuElem("North", AccelKey("KP_1"), sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 90, 0)));
    face_menu.push_back(MenuElem("South", sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 90, 180)));
    face_menu.push_back(MenuElem("West", AccelKey("KP_3"), sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 90, 270)));
    face_menu.push_back(MenuElem("East", sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 90, 90)));
    face_menu.push_back(MenuElem("Down", AccelKey("KP_7"), sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 0, 0)));
    face_menu.push_back(MenuElem("Up", sigc::bind<float, float>(sigc::mem_fun(*m_glarea, &GlView::setFace), 180, 0)));
    view_menu.push_back(MenuElem("Face..", *menu_sub_sub));
    view_menu.push_back(MenuElem("Camera Control..", sigc::mem_fun(*m_glarea, &GlView::showCameraControl)));
    view_menu.push_back(SeparatorElem());
    view_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::NEW), sigc::bind<Model*>(sigc::mem_fun(m_mainWindow, &MainWindow::newView),&m_model)));

    menuitem = manage( new Gtk::MenuItem("View") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& layer_menu = menu_sub->items();
    layer_menu.push_back(TearoffMenuElem());
    layer_menu.push_back(MenuElem("Layers...", sigc::mem_fun(m_mainWindow, &MainWindow::layer_window)));

    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& current_layer_menu = menu_sub_sub->items();
    current_layer_menu.push_back(StockMenuElem(Gtk::StockID(Gtk::Stock::PROPERTIES), sigc::mem_fun(m_model, &Model::options)));
    current_layer_menu.push_back(SeparatorElem());
    current_layer_menu.push_back(MenuElem("Default", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::DEFAULT)));
    current_layer_menu.push_back(MenuElem("Line", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::LINE)));
    current_layer_menu.push_back(MenuElem("Solid", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::SOLID)));
    current_layer_menu.push_back(MenuElem("Shaded", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::SHADED)));
    current_layer_menu.push_back(MenuElem("Textured", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::TEXTURE)));
    current_layer_menu.push_back(MenuElem("Lit", sigc::bind<GlView::rmode_t>(sigc::mem_fun(*m_glarea, &GlView::setLayerRenderMode),GlView::SHADETEXT)));

    layer_menu.push_back(SeparatorElem());
    layer_menu.push_back(MenuElem("Current Layer", *menu_sub_sub));

    menuitem = manage( new Gtk::MenuItem("Layers") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& tools_menu = menu_sub->items();
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& generic_tools_menu = menu_sub_sub->items();
    Gtk::RadioMenuItem::Group generic_tools_group;
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Select single", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::SELECT)));
    static_cast<Gtk::RadioMenuItem*>(&generic_tools_menu.back())->set_active();
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Select area", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::AREA)));
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Insert", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::DRAW)));
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Rotate", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::ROTATE)));
    generic_tools_menu.back().set_sensitive(false);
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Scale", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::SCALE)));
    generic_tools_menu.back().set_sensitive(false);
    generic_tools_menu.push_back(RadioMenuElem(generic_tools_group, "Translate", sigc::bind(sigc::mem_fun(m_mainWindow,&MainWindow::toolSelect),MainWindow::MOVE)));
    tools_menu.push_back(MenuElem("Generic", *menu_sub_sub));

    menuitem = manage( new Gtk::MenuItem("Tools") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& filters_menu = menu_sub->items();
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& entity_filters_menu = menu_sub_sub->items();
    entity_filters_menu.push_back(MenuElem("Align to heightmap"));
    filters_menu.push_back(MenuElem("Entity", *menu_sub_sub));
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& tile_filters_menu = menu_sub_sub->items();
    tile_filters_menu.push_back(MenuElem("Do tily thing"));
    tile_filters_menu.back().set_sensitive(false);
    filters_menu.push_back(MenuElem("Tile", *menu_sub_sub));
    menu_sub_sub = manage( new Gtk::Menu() );
    MenuList& height_filters_menu = menu_sub_sub->items();
    height_filters_menu.push_back(MenuElem("Match edges"));
    height_filters_menu.back().set_sensitive(false);
    filters_menu.push_back(MenuElem("HeightMap", *menu_sub_sub));

    menuitem = manage( new Gtk::MenuItem("Filters") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    menu_sub = manage( new Gtk::Menu() );
    MenuList& net_menu = menu_sub->items();
    net_menu.push_back(TearoffMenuElem());
    net_menu.push_back(MenuElem("Connect.."));
    net_menu.back().set_sensitive(false);
    net_menu.push_back(MenuElem("Disconnect..."));
    net_menu.back().set_sensitive(false);

    menuitem = manage( new Gtk::MenuItem("Net") );
    menuitem->set_submenu(*menu_sub);
    menubar->append(*menuitem);

    m_vAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_vAdjust->signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::vAdjustChanged));
    m_hAdjust = manage( new Gtk::Adjustment(0, -500, 500, 1, 25, 10) );
    m_hAdjust->signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::hAdjustChanged));

    Gtk::Table * table = manage( new Gtk::Table(3, 3, false) );
    m_vRuler = manage( new Gtk::VRuler() );
    table->attach(*m_vRuler, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
    m_hRuler = manage( new Gtk::HRuler() );
    table->attach(*m_hRuler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
    m_vScrollbar = manage( new Gtk::VScrollbar(*m_vAdjust) );
    table->attach(*m_vScrollbar, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
    m_hScrollbar = manage( new Gtk::HScrollbar(*m_hAdjust) );
    table->attach(*m_hScrollbar, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);
    table->attach(*m_glarea, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
                                         Gtk::FILL | Gtk::EXPAND);

    vbox->pack_start(*table);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );

    hbox->pack_start(*m_cursorCoords, Gtk::PACK_EXPAND_WIDGET, 2);
    hbox->pack_start(*m_viewCoords, Gtk::PACK_EXPAND_WIDGET, 2);

    vbox->pack_start(*hbox, Gtk::PACK_SHRINK);

    add(*vbox);

    cursorMoved();
    updateViewCoords();

    show_all();

    m_glarea->getDeclinationAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    m_glarea->getRotationAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    m_glarea->getScaleAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    // Dargh! looping evil!
    m_glarea->getXAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    m_glarea->getYAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    m_glarea->getZAdjustment().signal_value_changed().connect(sigc::mem_fun(*this, &ViewWindow::glViewChanged));
    glViewChanged();
    signal_delete_event().connect(sigc::mem_fun(*this, &ViewWindow::deleteEvent));
    m_glarea->signal_motion_notify_event().connect_notify(sigc::mem_fun(*this, &ViewWindow::motionNotifyEvent));
}

void ViewWindow::setTitle()
{
    if (m_glarea == NULL) {
        set_title("");
    } else {
        set_title(m_glarea->m_model.getName() + m_glarea->details());
    }
}

void ViewWindow::cursorMoved()
{
    std::stringstream text;
    float x, y, z;
    m_glarea->m_model.getCursor(x, y, z);
    text << " " << x << "," << y << "," << z << " ";
    m_cursorCoords->pop(m_cursorContext);
    m_cursorCoords->push(text.str(), m_cursorContext);
}

void ViewWindow::vAdjustChanged()
{
    if (m_scrollLock == true) { return; }
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    updateViewCoords();
}

void ViewWindow::hAdjustChanged()
{
    if (m_scrollLock == true) { return; }
    m_glarea->setViewOffset(-m_hAdjust->get_value(),
                            m_vAdjust->get_value(),
                            m_dAdjust);
    updateViewCoords();
}

void ViewWindow::glViewChanged()
{
    m_scrollLock = true;
    debug(std::cout << "Changing sliders to take accout of view change"
                    << std::endl << std::flush;);
    float wx, wy, wz;
    m_glarea->m_model.getSize(wx, wy, wz);
    float winx = m_glarea->get_width() / (40.0f * m_glarea->getScale());
    float winy = m_glarea->get_height() / (40.0f * m_glarea->getScale());
    float worldSize = hypot(wx, wy);

    debug(std::cout << "SLIDERS: " << winx << "," << winy << " (" << worldSize
                    << ") " << std::endl << std::flush;);

    m_hAdjust->set_lower(-worldSize);
    m_vAdjust->set_lower(-worldSize);
    m_hAdjust->set_upper(worldSize + winx);
    m_vAdjust->set_upper(worldSize + winx);
    
    m_hAdjust->set_page_size(winx);
    m_vAdjust->set_page_size(winx);

    m_hAdjust->set_page_increment(winx);
    m_vAdjust->set_page_increment(winy);

    m_hAdjust->set_step_increment(winx / 10.0f);
    m_vAdjust->set_step_increment(winy / 10.0f);

    float th, tv;
    m_glarea->getViewOffset(th, tv, m_dAdjust);
    m_hAdjust->set_value(-th);
    m_vAdjust->set_value(tv);

    updateRulers();
    m_scrollLock = false;
}

void ViewWindow::updateViewCoords()
{
    float x = m_glarea->getXoff();
    float y = m_glarea->getYoff();
    float z = m_glarea->getZoff();
    std::stringstream text;
    text << " " << x << "," << y << "," << z << " ";
    m_viewCoords->pop(m_viewContext);
    m_viewCoords->push(text.str(), m_viewContext);
    updateRulers();
}

void ViewWindow::updateRulers()
{
    float th = m_hAdjust->get_value(), tv = m_vAdjust->get_value();
    float winx = m_glarea->get_width() / (40.0f * m_glarea->getScale());
    float winy = m_glarea->get_height() / (40.0f * m_glarea->getScale());
    float hrl = th - winx/2,
          hrh = th + winx/2,
          vrl = - tv + winy/2,
          vrh = - tv - winy/2;
    float posx = hrl + m_glarea->mousex / (40.f * m_glarea->getScale()),
          posy = vrl - m_glarea->mousey / (40.f * m_glarea->getScale());

    m_hRuler->set_range(hrl, hrh, posx, hrh);
    m_hRuler->draw_ticks();
    m_vRuler->set_range(vrl, vrh, posy, vrh);
    m_vRuler->draw_ticks();
}

void ViewWindow::motionNotifyEvent(GdkEventMotion * event)
{
    double low, up, pos, max;

    m_hRuler->get_range(low, up, pos, max);
    pos = low + event->x / (40.f * m_glarea->getScale()),
    m_hRuler->set_range(low, up, pos, max);

    m_vRuler->get_range(low, up, pos, max);
    pos = low - event->y / (40.f * m_glarea->getScale());
    m_vRuler->set_range(low, up, pos, max);
}

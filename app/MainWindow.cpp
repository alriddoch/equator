// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "MainWindow.h"
#include "ViewWindow.h"
#include "LayerWindow.h"
#include "InheritanceWindow.h"
#include "ServerWindow.h"
#include "Model.h"
#include "Palette.h"

#include "gui/gtkmm/DockWindow.h"

#include <gtkmm/main.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/tearoffmenuitem.h>
#include <gtkmm/menubar.h>
#include <gtkmm/button.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/tooltips.h>

#include <gdkmm/pixmap.h>

#include <gtk/gtkwindow.h>

#include "../arrow.xpm"
#include "../select.xpm"
#include "../draw.xpm"
#include "../rotate.xpm"
#include "../scale.xpm"
#include "../move.xpm"
#include "../entity.xpm"
#include "../vertex.xpm"

#include <iostream>

MainWindow::MainWindow() :
    m_tool(MainWindow::SELECT), m_toolMode(MainWindow::ENTITY),
    m_layerwindow (*new LayerWindow(*this) ),
    m_inheritancewindow (*new InheritanceWindow(*this) ),
    m_serverwindow (*new ServerWindow(*this) ),
    m_palettewindow (*new Palette(*this) )
{
    signal_delete_event().connect(slot(*this, &MainWindow::destroy_handler));

    Gtk::Menu * menu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& file_menu = menu->items();
    file_menu.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_New", Gtk::Menu_Helpers::AccelKey('n', Gdk::CONTROL_MASK), SigC::slot(*this, &MainWindow::menuNewModel)));
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_Open...", Gtk::Menu_Helpers::AccelKey('o',Gdk::CONTROL_MASK)));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Connect...", slot(*this, &MainWindow::new_server_dialog)));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Preferences..."));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());

    Gtk::Menu * menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& window_sub = menu_sub->items();
    window_sub.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Layers...", slot(*this, &MainWindow::layer_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Inheritance...", slot(*this, &MainWindow::inheritance_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Servers...", slot(*this, &MainWindow::server_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Entity palette...", slot(*this, &MainWindow::palette_window)));

    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Windows", *menu_sub));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_Quit", Gtk::Menu_Helpers::AccelKey('q',Gdk::CONTROL_MASK), slot(*this, &MainWindow::menu_quit)));

    menu->accelerate(*this);
    //Gtk::MenuItem * menu_items = manage( new Gtk::MenuItem("New") );
    //menu->append(*menu_items);
    //menu_items->activate.connect(slot(this, &MainWindow::newModel));

    //menu_items = manage( new Gtk::MenuItem("Load...") );
    //menu->append(*menu_items);

    Gtk::MenuItem * menu_root = manage( new Gtk::MenuItem("File") );

    menu_root->set_submenu(*menu);

    Gtk::MenuBar * menu_bar = manage( new Gtk::MenuBar() );

    menu_bar->append(*menu_root);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    vbox->pack_start(*menu_bar, Gtk::PACK_SHRINK, 0);

    Gtk::Table * table = manage( new Gtk::Table(5, 2, true) );

    Gtk::ToggleButton * b = select_tool = manage( new Gtk::ToggleButton() );
    b->set_active(true); // Do this before we connect to the signal
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::SELECT));
    Glib::RefPtr<Gdk::Bitmap> pixmask;
    Glib::RefPtr<Gdk::Pixmap> p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, arrow_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 0, 1);
    Gtk::Tooltips * t = manage( new Gtk::Tooltips() );
    t->set_tip(*b, "Select Item");

    b = area_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::AREA));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, select_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 1, 2, 0, 1);
    t->set_tip(*b, "Select Area");

    b = draw_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::DRAW));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, draw_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 2, 3, 0, 1);
    t->set_tip(*b, "Insert");

    b = rotate_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::ROTATE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, rotate_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 3, 4, 0, 1);
    t->set_tip(*b, "Rotate");

    b = scale_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::SCALE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, scale_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 4, 5, 0, 1);
    t->set_tip(*b, "Scale");

    b = move_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::toolSelect),MainWindow::MOVE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, move_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 1, 2);
    t->set_tip(*b, "Translate");

    b = entity_mode = manage( new Gtk::ToggleButton() );
    b->set_active(true); // Do this before we connect to the signal
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::modeSelect),MainWindow::ENTITY));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, entity_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 2, 3);
    t->set_tip(*b, "Entity Mode");

    b = vertex_mode = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(bind(slot(*this,&MainWindow::modeSelect),MainWindow::VERTEX));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, vertex_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 1, 2, 2, 3);
    t->set_tip(*b, "Vertex Mode");

    b = manage( new Gtk::ToggleButton("7") );
    table->attach(*b, 0, 1, 3, 4);
    b = manage( new Gtk::ToggleButton("8") );
    table->attach(*b, 1, 2, 3, 4);
    b = manage( new Gtk::ToggleButton("9") );
    table->attach(*b, 2, 3, 3, 4);
    b = manage( new Gtk::ToggleButton("10") );
    table->attach(*b, 3, 4, 3, 4);

    vbox->pack_start(*table, Gtk::PACK_SHRINK);

    add(*vbox);

    set_title("Equator");

    // Gtk::Main::timeout.connect(slot(this, &MainWindow::idle), 1000);

    show_all();

}

gint MainWindow::quit(GdkEventAny *)
{
    return 0;
}

bool MainWindow::destroy_handler(GdkEventAny*)
{
    Gtk::Main::quit();
    return 0;
}

gint MainWindow::idle()
{
    std::cout << "IDLE" << std::endl << std::flush;
    return 1;
}

void MainWindow::menuNewModel()
{
    newModel();
}

Model & MainWindow::newModel()
{
    Model * model = new Model(*this);
    ViewWindow * view = new ViewWindow(*this, *model);
    model->setName("Untitled");
    m_views.push_back(view);
    m_models.push_back(model);
    modelAdded.emit(model);

    return *model;
}

void MainWindow::newView(Model * model)
{
    ViewWindow * view = manage( new ViewWindow(*this, *model) );
    m_views.push_back(view);
    view->setTitle();
}

void MainWindow::menu_quit()
{
    Gtk::Main::quit();
}

void MainWindow::showOptionBox(OptionBox & ob)
{
    Gtk::Window * w = ob.getDock();
    if (w == 0) {
        DockWindow * dw = new DockWindow(ob);
        dw->show_all();
    } else {
        if (!w->is_visible()) {
            w->show();
        }
    }
}

void MainWindow::inheritance_window()
{
    showOptionBox(m_inheritancewindow);
}

void MainWindow::server_window()
{
    showOptionBox(m_serverwindow);
}

void MainWindow::palette_window()
{
    showOptionBox(m_palettewindow);
}

void MainWindow::layer_window()
{
    showOptionBox(m_layerwindow);
}

void MainWindow::new_server_dialog()
{
    server_window();
    m_serverwindow.connect();
}

void MainWindow::toolSelect(MainWindow::toolType tool)
{
    static bool changing = false;
    if (!changing) {
        changing = true;
        m_tool = tool;
        select_tool->set_active(tool==MainWindow::SELECT);
        area_tool->set_active(tool==MainWindow::AREA);
        draw_tool->set_active(tool==MainWindow::DRAW);
        rotate_tool->set_active(tool==MainWindow::ROTATE);
        scale_tool->set_active(tool==MainWindow::SCALE);
        move_tool->set_active(tool==MainWindow::MOVE);
        changing = false;
    }
}

void MainWindow::modeSelect(MainWindow::toolMode mode)
{
    static bool changing = false;
    if (!changing) {
        changing = true;
        m_toolMode = mode;
        entity_mode->set_active(mode==MainWindow::ENTITY);
        vertex_mode->set_active(mode==MainWindow::VERTEX);
        changing = false;
    }
}

void MainWindow::setCurrentModel(Model * m)
{
    currentModelChanged.emit(m);
}

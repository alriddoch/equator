// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "MainWindow.h"
#include "ViewWindow.h"
#include "LayerWindow.h"
#include "InheritanceWindow.h"
#include "ServerWindow.h"
#include "Model.h"
#include "Server.h"
#include "Palette.h"

#include "gui/gtkmm/EntityTree.h"
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
#include <gtkmm/stock.h>

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

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <iostream>

MainWindow::MainWindow() :
    m_tool(MainWindow::SELECT), m_toolMode(MainWindow::ENTITY),
    m_layerwindow (*new LayerWindow(*this) ),
    m_inheritancewindow (*new InheritanceWindow(*this) ),
    m_serverwindow (*new ServerWindow(*this) ),
    m_palettewindow (*new Palette(*this) ),
    m_entitytreewindow (*new EntityTree(*this) )
{
    signal_delete_event().connect(SigC::slot(*this, &MainWindow::destroy_handler));

    Gtk::Menu * menu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& file_menu = menu->items();
    file_menu.push_back(Gtk::Menu_Helpers::TearoffMenuElem());

    Gtk::Menu * menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& new_sub = menu_sub->items();
    new_sub.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    new_sub.push_back(Gtk::Menu_Helpers::MenuElem("Empty Project", SigC::slot(*this, &MainWindow::menuNewModel)));
    new_sub.push_back(Gtk::Menu_Helpers::MenuElem("Server Connection...", SigC::slot(*this, &MainWindow::new_server_dialog)));

    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("New", *menu_sub));
    // file_menu.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID(Gtk::Stock::NEW), SigC::slot(*this, &MainWindow::menuNewModel)));
    file_menu.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID(Gtk::Stock::OPEN)));
    file_menu.back().set_sensitive(false);
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID(Gtk::Stock::PREFERENCES)));
    file_menu.back().set_sensitive(false);
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& window_sub = menu_sub->items();
    window_sub.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Layers...", SigC::slot(*this, &MainWindow::layer_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Inheritance...", SigC::slot(*this, &MainWindow::inheritance_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Servers...", SigC::slot(*this, &MainWindow::server_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Entity palette...", SigC::slot(*this, &MainWindow::palette_window)));
    window_sub.push_back(Gtk::Menu_Helpers::MenuElem("Entity tree...", SigC::slot(*this, &MainWindow::entity_tree_window)));

    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Windows", *menu_sub));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID(Gtk::Stock::QUIT), SigC::slot(*this, &MainWindow::menu_quit)));

    menu->accelerate(*this);

    Gtk::MenuItem * menu_root = manage( new Gtk::MenuItem("File") );
    menu_root->set_submenu(*menu);
    Gtk::MenuBar * menu_bar = manage( new Gtk::MenuBar() );
    menu_bar->append(*menu_root);

    menu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& help_menu = menu->items();
    help_menu.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    help_menu.push_back(Gtk::Menu_Helpers::MenuElem("About"/*, SigC::slot(*this, &MainWindow::layer_window)*/));

    menu_root = manage( new Gtk::MenuItem("Help") );
    menu_root->set_submenu(*menu);
    menu_bar->append(*menu_root);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    vbox->pack_start(*menu_bar, Gtk::PACK_SHRINK, 0);

    Gtk::Table * table = manage( new Gtk::Table(5, 2, true) );

    Gtk::ToggleButton * b = select_tool = manage( new Gtk::ToggleButton() );
    b->set_active(true); // Do this before we connect to the signal
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::SELECT));
    Glib::RefPtr<Gdk::Bitmap> pixmask;
    Glib::RefPtr<Gdk::Pixmap> p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, arrow_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 0, 1);
    Gtk::Tooltips * t = manage( new Gtk::Tooltips() );
    t->set_tip(*b, "Select Item");

    b = area_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::AREA));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, select_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 1, 2, 0, 1);
    t->set_tip(*b, "Select Area");

    b = draw_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::DRAW));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, draw_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 2, 3, 0, 1);
    t->set_tip(*b, "Insert");

    b = rotate_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::ROTATE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, rotate_xpm);
    b->add_pixmap(p, pixmask);
    b->set_sensitive(false);
    table->attach(*b, 3, 4, 0, 1);
    t->set_tip(*b, "Rotate");

    b = scale_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::SCALE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, scale_xpm);
    b->add_pixmap(p, pixmask);
    b->set_sensitive(false);
    table->attach(*b, 4, 5, 0, 1);
    t->set_tip(*b, "Scale");

    b = move_tool = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::toolSelect),MainWindow::MOVE));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, move_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 1, 2);
    t->set_tip(*b, "Translate");

    b = entity_mode = manage( new Gtk::ToggleButton() );
    b->set_active(true); // Do this before we connect to the signal
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::modeSelect),MainWindow::ENTITY));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, entity_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 0, 1, 2, 3);
    t->set_tip(*b, "Entity Mode");

    b = vertex_mode = manage( new Gtk::ToggleButton() );
    b->signal_clicked().connect(SigC::bind(SigC::slot(*this,&MainWindow::modeSelect),MainWindow::VERTEX));
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, vertex_xpm);
    b->add_pixmap(p, pixmask);
    table->attach(*b, 1, 2, 2, 3);
    t->set_tip(*b, "Vertex Mode");

    b = manage( new Gtk::ToggleButton("7") );
    b->set_sensitive(false);
    table->attach(*b, 0, 1, 3, 4);
    b = manage( new Gtk::ToggleButton("8") );
    b->set_sensitive(false);
    table->attach(*b, 1, 2, 3, 4);
    b = manage( new Gtk::ToggleButton("9") );
    b->set_sensitive(false);
    table->attach(*b, 2, 3, 3, 4);
    b = manage( new Gtk::ToggleButton("10") );
    b->set_sensitive(false);
    table->attach(*b, 3, 4, 3, 4);

    vbox->pack_start(*table, Gtk::PACK_SHRINK);

    add(*vbox);

    set_title("Equator");

    // Gtk::Main::timeout.connect(SigC::slot(this, &MainWindow::idle), 1000);

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

Server & MainWindow::newServer(const std::string & name)
{
    Server * server = new Server(*this, name);

    m_servers.push_back(server);
    serverAdded.emit(server);

    return *server;
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

void MainWindow::entity_tree_window()
{
    showOptionBox(m_entitytreewindow);
}

void MainWindow::new_server_dialog()
{
    server_window();
    m_serverwindow.connect();
}

void MainWindow::toolSelect(MainWindow::ToolType tool)
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
        toolChanged.emit();
    }
}

void MainWindow::modeSelect(MainWindow::ToolMode mode)
{
    static bool changing = false;
    if (!changing) {
        changing = true;
        m_toolMode = mode;
        entity_mode->set_active(mode==MainWindow::ENTITY);
        vertex_mode->set_active(mode==MainWindow::VERTEX);
        changing = false;
        modeChanged.emit();
    }
}

void MainWindow::setCurrentModel(Model * m)
{
    currentModelChanged.emit(m);
}

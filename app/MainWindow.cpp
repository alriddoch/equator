// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "MainWindow.h"
#include "ViewWindow.h"
#include "LayerWindow.h"
#include "InheritanceWindow.h"
#include "ServerWindow.h"
#include "NewServerWindow.h"
#include "Model.h"

#include <gtk--/main.h>
#include <gtk--/menu.h>
#include <gtk--/menuitem.h>
#include <gtk--/tearoffmenuitem.h>
#include <gtk--/menubar.h>
#include <gtk--/button.h>
#include <gtk--/togglebutton.h>
#include <gtk--/box.h>
#include <gtk--/pixmap.h>
#include <gtk--/table.h>

#include <iostream>

MainWindow::MainWindow() : Gtk::Window(GTK_WINDOW_TOPLEVEL), m_tool(MainWindow::SELECT)
{
    destroy.connect(slot(this, &MainWindow::destroy_handler));

    Gtk::Menu * menu = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& file_menu = menu->items();
    file_menu.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_New", Gtk::Menu_Helpers::CTL|'n', slot(this, &MainWindow::new_view)));
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_Open...", Gtk::Menu_Helpers::CTL|'o'));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Connect...", slot(this, &MainWindow::new_server_dialog)));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Preferences..."));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());

    Gtk::Menu * menu_sub = manage( new Gtk::Menu() );
    Gtk::Menu_Helpers::MenuList& dialog_sub = menu_sub->items();
    dialog_sub.push_back(Gtk::Menu_Helpers::TearoffMenuElem());
    dialog_sub.push_back(Gtk::Menu_Helpers::MenuElem("Layers...", SigC::bind<Model*>(slot(this, &MainWindow::open_layers),NULL)));
    dialog_sub.push_back(Gtk::Menu_Helpers::MenuElem("Inheritance...", slot(this, &MainWindow::inheritance_dialog)));
    dialog_sub.push_back(Gtk::Menu_Helpers::MenuElem("Servers...", slot(this, &MainWindow::server_dialog)));
    dialog_sub.push_back(Gtk::Menu_Helpers::MenuElem("Entities..."));

    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("Dialogs", *menu_sub));
    file_menu.push_back(Gtk::Menu_Helpers::SeparatorElem());
    file_menu.push_back(Gtk::Menu_Helpers::MenuElem("_Quit", Gtk::Menu_Helpers::CTL|'q', slot(this, &MainWindow::menu_quit)));

    menu->accelerate(*this);
    //Gtk::MenuItem * menu_items = manage( new Gtk::MenuItem("New") );
    //menu->append(*menu_items);
    //menu_items->activate.connect(slot(this, &MainWindow::new_view));

    //menu_items = manage( new Gtk::MenuItem("Load...") );
    //menu->append(*menu_items);

    Gtk::MenuItem * menu_root = manage( new Gtk::MenuItem("File") );

    menu_root->set_submenu(*menu);

    Gtk::MenuBar * menu_bar = manage( new Gtk::MenuBar() );

    menu_bar->append(*menu_root);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    vbox->pack_start(*menu_bar, false, false, 0);

    Gtk::Table * table = manage( new Gtk::Table(5, 2, true) );
    Gtk::ToggleButton * b = select_tool = manage( new Gtk::ToggleButton() );
    b->set_active(true); // Do this before we connect to the signal
    b->clicked.connect(bind(slot(this,&MainWindow::toolSelect),MainWindow::SELECT));

    Gtk::Pixmap * p = manage( new Gtk::Pixmap("arrow.xpm") );
    b->add(*p);
    table->attach(*b, 0, 1, 0, 1);
    b = area_tool = manage( new Gtk::ToggleButton() );
    b->clicked.connect(bind(slot(this,&MainWindow::toolSelect),MainWindow::AREA));
    p = manage( new Gtk::Pixmap("select.xpm") );
    b->add(*p);
    table->attach(*b, 1, 2, 0, 1);
    b = vertex_tool = manage( new Gtk::ToggleButton() );
    b->clicked.connect(bind(slot(this,&MainWindow::toolSelect),MainWindow::VERTEX));
    p = manage( new Gtk::Pixmap("vertex.xpm") );
    b->add(*p);
    table->attach(*b, 2, 3, 0, 1);
    b = tiler_tool = manage( new Gtk::ToggleButton() );
    b->clicked.connect(bind(slot(this,&MainWindow::toolSelect),MainWindow::TILER));
    p = manage( new Gtk::Pixmap("tile.xpm") );
    b->add(*p);
    table->attach(*b, 3, 4, 0, 1);
    b = move_tool = manage( new Gtk::ToggleButton() );
    b->clicked.connect(bind(slot(this,&MainWindow::toolSelect),MainWindow::MOVE));
    p = manage( new Gtk::Pixmap("move.xpm") );
    b->add(*p);
    table->attach(*b, 4, 5, 0, 1);
    b = manage( new Gtk::ToggleButton("6") );
    table->attach(*b, 0, 1, 1, 2);
    b = manage( new Gtk::ToggleButton("7") );
    table->attach(*b, 1, 2, 1, 2);
    b = manage( new Gtk::ToggleButton("8") );
    table->attach(*b, 2, 3, 1, 2);
    b = manage( new Gtk::ToggleButton("9") );
    table->attach(*b, 3, 4, 1, 2);
    b = manage( new Gtk::ToggleButton("10") );
    table->attach(*b, 4, 5, 1, 2);

    vbox->pack_end(*table, true, true, 0);

    add(*vbox);

    show_all();

    m_layerwindow = manage( new LayerWindow(*this) );
    m_inheritancewindow = manage( new InheritanceWindow(*this) );
    m_serverwindow = manage( new ServerWindow(*this) );
    m_newServerwindow = manage( new NewServerWindow(*this) );
}

gint MainWindow::quit(GdkEventAny *)
{
    return 0;
}

void MainWindow::destroy_handler()
{
    // This is called from the detructor, so must not contain anything else
    Gtk::Main::quit();
}

void MainWindow::new_view()
{
    Model * model = new Model(*this);
    ViewWindow * view = manage( new ViewWindow(*this, *model) );
    model->setName("Untitled");
    m_views.push_back(view);
    m_models.push_back(model);
    modelAdded.emit(model);
    
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

void MainWindow::inheritance_dialog()
{
    m_inheritancewindow->show_all();
}

void MainWindow::server_dialog()
{
    m_serverwindow->show_all();
}

void MainWindow::new_server_dialog()
{
    m_newServerwindow->show_all();
}

void MainWindow::open_layers(Model * model)
{
    if (model != NULL) {
        m_layerwindow->setModel(model);
    }
    m_layerwindow->show_all();
}

void MainWindow::toolSelect(MainWindow::toolType tool)
{
    static bool changing = false;
    if (!changing) {
        changing = true;
        m_tool = tool;
        select_tool->set_active(tool==MainWindow::SELECT);
        area_tool->set_active(tool==MainWindow::AREA);
        vertex_tool->set_active(tool==MainWindow::VERTEX);
        tiler_tool->set_active(tool==MainWindow::TILER);
        move_tool->set_active(tool==MainWindow::MOVE);
        changing = false;
    }
}

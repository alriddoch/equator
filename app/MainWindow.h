// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_MAINWINDOW_H
#define EQUATOR_APP_MAINWINDOW_H

#include <gtk--/window.h>
#include <gtk--/menu.h>

#include <list>

class ViewWindow;
class LayerWindow;
class InheritanceWindow;
class ServerWindow;
class GlView;

class MainWindow : public Gtk::Window
{
  private:
    //Gtk::Menu * m_menu;
    std::list<ViewWindow*> m_views;
    LayerWindow * m_layerwindow;
    InheritanceWindow * m_inheritancewindow;
    ServerWindow * m_serverwindow;

    void destroy_handler();
  public:
    MainWindow();

    LayerWindow * getLayerWindow() {
        return m_layerwindow;
    }

    gint quit( GdkEventAny *);
    void new_view();
    void menu_quit();
    void inheritance_dialog();
    void server_dialog();
    void open_layers(GlView *);

};

#endif // EQUATOR_APP_MAINWINDOW_H

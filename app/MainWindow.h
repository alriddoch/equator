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

namespace Gtk {
  class ToggleButton;
}

class MainWindow : public Gtk::Window
{
  private:
    //Gtk::Menu * m_menu;
    std::list<ViewWindow*> m_views;
    LayerWindow * m_layerwindow;
    InheritanceWindow * m_inheritancewindow;
    ServerWindow * m_serverwindow;

  public:
    typedef enum {
        SELECT, AREA, VERTEX, TILER, MOVE
    } toolType;

  private:
    toolType m_tool;

    Gtk::ToggleButton * select_tool;
    Gtk::ToggleButton * area_tool;
    Gtk::ToggleButton * vertex_tool;
    Gtk::ToggleButton * tiler_tool;
    Gtk::ToggleButton * move_tool;

    void destroy_handler();
  public:
    MainWindow();

    LayerWindow * getLayerWindow() {
        return m_layerwindow;
    }

    const toolType getTool() {
        return m_tool;
    }

    gint quit( GdkEventAny *);
    void new_view();
    void menu_quit();
    void inheritance_dialog();
    void server_dialog();
    void open_layers(GlView *);
    void toolSelect(toolType);

    SigC::Signal1<void, ViewWindow *> modelAdded;
};

#endif // EQUATOR_APP_MAINWINDOW_H

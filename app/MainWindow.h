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
class Palette;
class NewServerWindow;
class GlView;
class Model;
class Server;

namespace Gtk {
  class ToggleButton;
}

class MainWindow : public Gtk::Window
{
  private:
    //Gtk::Menu * m_menu;
    std::list<ViewWindow*> m_views;
    std::list<Model*> m_models;
    LayerWindow * m_layerwindow;
    InheritanceWindow * m_inheritancewindow;
    ServerWindow * m_serverwindow;
    NewServerWindow * m_newServerwindow;
    Palette * m_palettewindow;

  public:
    typedef enum {
        SELECT, AREA, DRAW, ROTATE, SCALE, MOVE
    } toolType;

    typedef enum {
        ENTITY, VERTEX
    } toolMode;

  private:
    toolType m_tool;
    toolMode m_toolMode;

    Gtk::ToggleButton * select_tool;
    Gtk::ToggleButton * area_tool;
    Gtk::ToggleButton * draw_tool;
    Gtk::ToggleButton * rotate_tool;
    Gtk::ToggleButton * scale_tool;
    Gtk::ToggleButton * move_tool;

    Gtk::ToggleButton * entity_mode;
    Gtk::ToggleButton * vertex_mode;

    void destroy_handler();
  public:
    Gtk::Main & m_main;

    MainWindow(Gtk::Main &);

    LayerWindow * getLayerWindow() {
        return m_layerwindow;
    }

    const toolType getTool() {
        return m_tool;
    }

    const toolMode getMode() {
        return m_toolMode;
    }

    gint quit( GdkEventAny *);
    void newModel();
    void newView(Model *);
    void menu_quit();
    void inheritance_dialog();
    void server_dialog();
    void palette();
    void new_server_dialog();
    void open_layers(Model *);
    void toolSelect(toolType);
    void modeSelect(toolMode);

    SigC::Signal1<void, Model *> modelAdded;
    SigC::Signal1<void, Server *> serverAdded;
    SigC::Signal1<void, Model *> currentModelChanged;
};

#endif // EQUATOR_APP_MAINWINDOW_H

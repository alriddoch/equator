// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_MAINWINDOW_H
#define EQUATOR_APP_MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/menu.h>

#include <list>

class ViewWindow;
class LayerWindow;
class InheritanceWindow;
class ServerWindow;
class Palette;
class Model;

namespace Gtk {
  class ToggleButton;
}

class MainWindow : public Gtk::Window
{
  private:
    //Gtk::Menu * m_menu;
    std::list<ViewWindow*> m_views;
    std::list<Model*> m_models;

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

    bool destroy_handler(GdkEventAny*);
    gint idle();

    void menuNewModel();
  public:
    // THese signals absolutely must be constructed before the other window
    // references, so that the other windows can connect in their constructors
    SigC::Signal1<void, Model *> modelAdded;
    SigC::Signal1<void, Model *> currentModelChanged;

    LayerWindow & m_layerwindow;
    InheritanceWindow & m_inheritancewindow;
    ServerWindow & m_serverwindow;
    Palette & m_palettewindow;

    MainWindow();

    const toolType getTool() {
        return m_tool;
    }

    const toolMode getMode() {
        return m_toolMode;
    }

    gint quit( GdkEventAny *);
    Model & newModel();
    void newView(Model *);
    void menu_quit();
    void inheritance_window();
    void server_window();
    void palette_window();
    void new_server_dialog();
    void layer_window();
    void toolSelect(toolType);
    void modeSelect(toolMode);
    void setCurrentModel(Model *);
};

#endif // EQUATOR_APP_MAINWINDOW_H

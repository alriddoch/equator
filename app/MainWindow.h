// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

#ifndef EQUATOR_APP_MAINWINDOW_H
#define EQUATOR_APP_MAINWINDOW_H

#include <sage/sage.h>

#include <gdkmm/gl/context.h>

#include <gtkmm/window.h>

#include <glibmm/timeval.h>

#include <list>

class ViewWindow;
class LayerWindow;
class InheritanceWindow;
class ServerWindow;
class Palette;
class Model;
class Server;
class OptionBox;
class EntityTree;

namespace Gtk {
  class ToggleButton;
  class FileChooserDialog;
}

class MainWindow : public Gtk::Window
{
  private:
    std::list<ViewWindow*> m_views;
    std::list<Model*> m_models;
    std::list<Server*> m_servers;

  public:
    typedef enum {
        SELECT, AREA, DRAW, ROTATE, SCALE, MOVE
    } ToolType;

    typedef enum {
        ENTITY, VERTEX
    } ToolMode;

    typedef enum {
        PAN, ORBIT, ZOOM
    } NavMode;

  private:
    ToolType m_tool;
    ToolMode m_toolMode;
    NavMode m_navMode;

    Glib::TimeVal m_time;

    Gtk::ToggleButton * select_tool;
    Gtk::ToggleButton * area_tool;
    Gtk::ToggleButton * draw_tool;
    Gtk::ToggleButton * rotate_tool;
    Gtk::ToggleButton * scale_tool;
    Gtk::ToggleButton * move_tool;

    Gtk::ToggleButton * entity_mode;
    Gtk::ToggleButton * vertex_mode;

    Gtk::ToggleButton * pan_mode;
    Gtk::ToggleButton * orbit_mode;
    Gtk::ToggleButton * zoom_mode;

    Gtk::FileChooserDialog * open_dialog;

    bool destroy_handler(GdkEventAny*);
    gint idle();

    void menuNewModel();
    void open_option();
    void open_response(int);
  public:
    // THese signals absolutely must be constructed before the other window
    // references, so that the other windows can connect in their constructors
    sigc::signal<void, Model *> modelAdded;
    sigc::signal<void, Model *> currentModelChanged;
    sigc::signal<void, Server *> serverAdded;
    sigc::signal<void> modeChanged;
    sigc::signal<void> toolChanged;
    sigc::signal<void> navChanged;

    LayerWindow & m_layerwindow;
    InheritanceWindow & m_inheritancewindow;
    ServerWindow & m_serverwindow;
    Palette & m_palettewindow;
    EntityTree & m_entitytreewindow;

    Glib::RefPtr<const Gdk::GL::Context> m_glContext;

    MainWindow();

    const ToolType getTool() {
        return m_tool;
    }

    const ToolMode getMode() {
        return m_toolMode;
    }

    const NavMode getNavMode() {
        return m_navMode;
    }

    const Glib::TimeVal & time() const {
        return m_time;
    }

    gint quit( GdkEventAny *);
    Model & newModel();
    void newView(Model *);
    Server & newServer(const std::string & name);
    void loadFile(const std::string & file);
    void showOptionBox(OptionBox &);
    void updateTime();
    void menu_quit();
    void inheritance_window();
    void server_window();
    void palette_window();
    void new_server_dialog();
    void layer_window();
    void entity_tree_window();
    void toolSelect(ToolType);
    void modeSelect(ToolMode);
    void navSelect(NavMode);
    void setCurrentModel(Model *);
};

#endif // EQUATOR_APP_MAINWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_MAINWINDOW_H
#define EQUATOR_APP_MAINWINDOW_H

#include <gtkmm/window.h>

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
    void showOptionBox(OptionBox &);
    void open_option();
    void open_response(int);
  public:
    // THese signals absolutely must be constructed before the other window
    // references, so that the other windows can connect in their constructors
    SigC::Signal1<void, Model *> modelAdded;
    SigC::Signal1<void, Model *> currentModelChanged;
    SigC::Signal1<void, Server *> serverAdded;
    SigC::Signal0<void> modeChanged;
    SigC::Signal0<void> toolChanged;
    SigC::Signal0<void> navChanged;

    LayerWindow & m_layerwindow;
    InheritanceWindow & m_inheritancewindow;
    ServerWindow & m_serverwindow;
    Palette & m_palettewindow;
    EntityTree & m_entitytreewindow;

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

    gint quit( GdkEventAny *);
    Model & newModel();
    void newView(Model *);
    Server & newServer(const std::string & name);
    void loadFile(const std::string & file);

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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LAYERWINDOW_H
#define EQUATOR_APP_LAYERWINDOW_H

#include <gtk--/window.h>
#include <gtk--/clist.h>

class MainWindow;
class ViewWindow;
class GlView;;
class NewLayerWindow;;
class Model;

namespace Gtk {
   class OptionMenu;
}

class LayerWindow : public Gtk::Window
{
  private:
    Gtk::CList * m_clist;
    Gtk::OptionMenu * m_modelMenu;
    Model * m_currentModel;
    NewLayerWindow * m_newLayerWindow;

  public:
    MainWindow & m_mainWindow;

    explicit LayerWindow(MainWindow &);

    void setModel(Model * model);
    void addModel(Model * view);
    void selectionMade(gint row, gint column, GdkEvent * event);

    void newLayerRequested();
    void raiseLayer();
    void lowerLayer();

};

#endif // EQUATOR_APP_LAYERWINDOW_H

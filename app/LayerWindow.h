// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LAYERWINDOW_H
#define EQUATOR_APP_LAYERWINDOW_H

#include <gtk--/window.h>
#include <gtk--/menu.h>
#include <gtk--/clist.h>

class MainWindow;
class GlView;;
class NewLayerWindow;;

class LayerWindow : public Gtk::Window
{
  private:
    Gtk::Menu * m_popup;
    Gtk::CList * m_clist;
    Gtk::Label * m_viewLabel;
    GlView * m_currentView;
    NewLayerWindow * m_newLayerWindow;

  public:
    MainWindow & m_mainWindow;

    explicit LayerWindow(MainWindow &);

    gint buttonEvent(GdkEventButton*);

    void setView(GlView * view);
    void newLayer();

};

#endif // EQUATOR_APP_LAYERWINDOW_H

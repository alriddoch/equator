// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_LAYERWINDOW_H
#define EQUATOR_APP_LAYERWINDOW_H

#include <gtkmm/window.h>

class MainWindow;
class ViewWindow;
class GlView;;
class NewLayerWindow;;
class Model;

namespace Gtk {
   class CList;
   class OptionMenu;
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
}

class LayerWindow : public Gtk::Window
{
  private:
    Glib::RefPtr<Gtk::ListStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > * m_visColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_typeColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Gtk::OptionMenu * m_modelMenu;
    GdkPixmap * m_eye;
    GdkBitmap * m_eyemask;
    GdkPixmap * m_null;
    GdkBitmap * m_nullmask;

    Model * m_currentModel;
    NewLayerWindow * m_newLayerWindow;
    SigC::Connection m_updateSignal;

  public:
    // MainWindow & m_mainWindow;

    explicit LayerWindow(MainWindow &);

    gint delete_event_impl(GdkEventAny*) {
        hide();
        return 1;
    }

    void setModel(Model * model);
    void updateLayers();
    void addModel(Model * view);
    void selectionMade(gint row, gint column, GdkEvent * event);

    void newLayerRequested();
    void raiseLayer();
    void lowerLayer();

};

#endif // EQUATOR_APP_LAYERWINDOW_H

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

#ifndef EQUATOR_APP_LAYERWINDOW_H
#define EQUATOR_APP_LAYERWINDOW_H

#include "gui/gtkmm/OptionBox.h"

class MainWindow;
class NewLayerWindow;;
class Model;
class Layer;

namespace Gtk {
   class OptionMenu;
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
}

class LayerWindow : public OptionBox
{
  private:
    Glib::RefPtr<Gtk::ListStore> m_treeModel;
    Gtk::TreeModelColumn<bool> * m_visColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_typeColumn;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Layer *> * m_ptrColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Gtk::OptionMenu * m_modelMenu;
    GdkPixmap * m_eye;
    GdkBitmap * m_eyemask;
    GdkPixmap * m_null;
    GdkBitmap * m_nullmask;

    Model * m_currentModel;
    NewLayerWindow * m_newLayerWindow;
    sigc::connection m_updateSignal;

    void visibleToggled(const Glib::ustring& path_string);
    void selectionChanged();

  public:
    // MainWindow & m_mainWindow;

    explicit LayerWindow(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void currentModelChanged(Model * model);
    void layersChanged();
    void modelAdded(Model * view);

    void newLayerRequested();
    void raiseLayer();
    void lowerLayer();

};

#endif // EQUATOR_APP_LAYERWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_NEWLAYERWINDOW_H
#define EQUATOR_APP_NEWLAYERWINDOW_H

#include <string>
#include <map>

class Layer;
class GlView;
class Model;

class LayerFactory {
  public:
    virtual Layer * newLayer(Model &) = 0;

    static std::map<std::string, LayerFactory *> factories;
    static Layer * newLayer(const std::string & type, Model & m) {
        std::map<std::string, LayerFactory *>::const_iterator I = factories.find(type);
        if (I == factories.end()) {
            return NULL;
        }
        return I->second->newLayer(m);
    }
};

template <typename T>
class LayerPlant : public LayerFactory {
  public:
    LayerPlant() { }
    virtual ~LayerPlant() { }
    Layer * newLayer(Model & m) { return new T(m); }
};

#include <gtkmm/dialog.h>

namespace Gtk {
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
}

class NewLayerWindow : public Gtk::Dialog
{
  private:
    Glib::RefPtr<Gtk::ListStore> m_treeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_treeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
    Model * m_currentModel;
    std::map<Glib::ustring, LayerFactory *> m_factories;
  public:
    NewLayerWindow();

    void doshow(Model * view);

    void okay();
    void cancel();
};

#endif // EQUATOR_APP_NEWLAYERWINDOW_H

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
    Layer * newLayer(Model & m) { return new T(m); }
};

#include <gtkmm/window.h>

namespace Gtk {
  class List;
}

class NewLayerWindow : public Gtk::Window
{
  private:
    Gtk::List * m_list;
    Model * m_currentModel;
  public:
    NewLayerWindow();

    gint delete_event_impl(GdkEventAny*) {
        hide();
        return 1;
    }

    void doshow(Model * view);

    void okay();
    void cancel();
};

#endif // EQUATOR_APP_NEWLAYERWINDOW_H

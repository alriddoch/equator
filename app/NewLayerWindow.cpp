// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewLayerWindow.h"
#include "Holo.h"
#include "BladeMap.h"
#include "IsoMap.h"
#include "Model.h"
#include "HeightManager.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

std::map<std::string, LayerFactory *> LayerFactory::factories;

class FactoryItem : public Gtk::ListItem {
  private:
    LayerFactory * m_factory;
  public:
    FactoryItem(LayerFactory * factory, char * label) : Gtk::ListItem(label),
                                                        m_factory(factory) {
    }

    LayerFactory * getFactory() const {
        return m_factory;
    }
};

NewLayerWindow::NewLayerWindow() : Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{
    m_list = manage( new Gtk::List() );

    LayerFactory * factory = new LayerPlant<Holo>();
    LayerFactory::factories["HoloWorld"] = factory;
    m_list->add(*(manage( new FactoryItem(factory, "HoloWorld") )));

    factory = new LayerPlant<BladeMap>();
    LayerFactory::factories["BladeMap"] = factory;
    m_list->add(*(manage( new FactoryItem(factory, "BladeMap") )));

    factory = new LayerPlant<IsoMap>();
    LayerFactory::factories["IsoMap"] = factory;
    m_list->add(*(manage( new FactoryItem(factory, "IsoMap") )));

    factory = new LayerPlant<HeightManager>();
    LayerFactory::factories["HeightManager"] = factory;
    m_list->add(*(manage( new FactoryItem(factory, "HeightManager") )));

    Gtk::ScrolledWindow * scrolled_window = manage( new Gtk::ScrolledWindow() );
    scrolled_window->set_usize(250,150);
    scrolled_window->add_with_viewport(*m_list);
    // scrolled_window->add(*m_list);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    vbox->pack_start(*scrolled_window, Gtk::EXPAND | Gtk::FILL, 2);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button("Okay") );
    b->clicked.connect(slot(*this, &NewLayerWindow::okay));
    hbox->pack_start(*b, Gtk::EXPAND | Gtk::FILL, 2);
    b = manage( new Gtk::Button("Cancel") );
    b->clicked.connect(slot(*this, &NewLayerWindow::cancel));
    hbox->pack_start(*b, Gtk::EXPAND | Gtk::FILL, 2);

    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 2);

    add(*vbox);
}

void NewLayerWindow::doshow(Model * model)
{
    m_currentModel = model;
    show_all();
}

void NewLayerWindow::okay()
{
    if (m_list->selection().empty()) {
        return;
    }
    FactoryItem * newLayerType = (FactoryItem *) *m_list->selection().begin();
    Layer * newLayer = newLayerType->getFactory()->newLayer(*m_currentModel);
    m_currentModel->addLayer(newLayer);
    hide();
}

void NewLayerWindow::cancel()
{
    hide();
}

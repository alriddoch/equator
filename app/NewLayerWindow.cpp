// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewLayerWindow.h"
#include "Holo.h"
#include "BladeMap.h"
#include "IsoMap.h"
#include "Model.h"
#include "HeightManager.h"
#include "Terrain.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/stock.h>
#include <gtkmm/stockitem.h>

#include <iostream>

std::map<std::string, LayerFactory *> LayerFactory::factories;

NewLayerWindow::NewLayerWindow() : Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{
    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_columns->add(*m_nameColumn);

    m_treeModel = Gtk::ListStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );
    m_treeView->append_column("Layer type", *m_nameColumn);

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);

    LayerFactory * factory = new LayerPlant<Holo>();
    LayerFactory::factories["HoloWorld"] = factory;
    m_factories["HoloWorld"] = factory;
    Gtk::TreeModel::Row row = *(m_treeModel->append());
    row[*m_nameColumn]     = "HoloWorld";
    //m_list->add(*(manage( new FactoryItem(factory, "HoloWorld") )));

    factory = new LayerPlant<BladeMap>();
    LayerFactory::factories["BladeMap"] = factory;
    m_factories["BladeMap"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "BladeMap";
    //m_list->add(*(manage( new FactoryItem(factory, "BladeMap") )));

    factory = new LayerPlant<IsoMap>();
    LayerFactory::factories["IsoMap"] = factory;
    m_factories["IsoMap"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "IsoMap";
    //m_list->add(*(manage( new FactoryItem(factory, "IsoMap") )));

    factory = new LayerPlant<HeightManager>();
    LayerFactory::factories["HeightManager"] = factory;
    m_factories["HeightManager"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "HeightManager";
    //m_list->add(*(manage( new FactoryItem(factory, "HeightManager") )));

    factory = new LayerPlant<Terrain>();
    LayerFactory::factories["Terrain"] = factory;
    m_factories["Terrain"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "Terrain";
    //m_list->add(*(manage( new FactoryItem(factory, "HeightManager") )));

    Gtk::ScrolledWindow * scrolled_window = manage( new Gtk::ScrolledWindow() );
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);
    // scrolled_window->add(*m_list);

    Gtk::VBox * vbox = manage( new Gtk::VBox() );
    vbox->pack_start(*scrolled_window, Gtk::EXPAND | Gtk::FILL, 2);

    Gtk::HBox * hbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button(Gtk::Stock::OK) );
    b->signal_clicked().connect(slot(*this, &NewLayerWindow::okay));
    hbox->pack_start(*b, Gtk::EXPAND | Gtk::FILL, 2);
    b = manage( new Gtk::Button(Gtk::Stock::CANCEL) );
    b->signal_clicked().connect(slot(*this, &NewLayerWindow::cancel));
    hbox->pack_start(*b, Gtk::EXPAND | Gtk::FILL, 2);

    vbox->pack_start(*hbox, Gtk::AttachOptions(0), 2);

    add(*vbox);

    signal_delete_event().connect(slot(*this, &NewLayerWindow::deleteEvent));
}

void NewLayerWindow::doshow(Model * model)
{
    m_currentModel = model;
    show_all();
}

void NewLayerWindow::okay()
{
    const Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
    if (!iter) {
        return;
    }
    const Gtk::TreeModel::Row row = *iter;

    // get the data out of the model
    const Glib::ustring name = row[*m_nameColumn];

    std::map<Glib::ustring, LayerFactory *>::const_iterator I =
        m_factories.find(name);
    if (I == m_factories.end()) {
        std::cerr << "BUG: factory " << name << " not found in factories"
                  << std::endl << std::flush;
        return;
    }

    Layer * newLayer = I->second->newLayer(*m_currentModel);
    m_currentModel->addLayer(newLayer);
    hide();
}

void NewLayerWindow::cancel()
{
    hide();
}

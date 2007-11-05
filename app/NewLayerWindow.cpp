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

#include "NewLayerWindow.h"
#include "Holo.h"
#include "Model.h"
#include "Terrain.h"
#include "Cal3dStore.h"
#include "Forest.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/stock.h>

#include <sigc++/functors/mem_fun.h>

#include <iostream>

std::map<std::string, LayerFactory *> LayerFactory::factories;

NewLayerWindow::NewLayerWindow()
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

    factory = new LayerPlant<Terrain>();
    LayerFactory::factories["Terrain"] = factory;
    m_factories["Terrain"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "Terrain";

    factory = new LayerPlant<Cal3dStore>();
    LayerFactory::factories["Cal3d"] = factory;
    m_factories["Cal3d"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "Cal3d";

#if 0
    factory = new LayerPlant<Forest>();
    LayerFactory::factories["Forest"] = factory;
    m_factories["Forest"] = factory;
    row = *(m_treeModel->append());
    row[*m_nameColumn]     = "Forest";
#endif

    Gtk::ScrolledWindow * scrolled_window = manage( new Gtk::ScrolledWindow() );
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    get_vbox()->pack_start(*scrolled_window);

    Gtk::Button * b = add_button(Gtk::Stock::CANCEL , Gtk::RESPONSE_CANCEL);
    b->signal_clicked().connect(sigc::mem_fun(*this, &NewLayerWindow::cancel));
    b = add_button(Gtk::Stock::OK , Gtk::RESPONSE_OK);
    b->signal_clicked().connect(sigc::mem_fun(*this, &NewLayerWindow::okay));
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

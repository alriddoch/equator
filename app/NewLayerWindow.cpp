// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "NewLayerWindow.h"
#include "Holo.h"
#include "Model.h"
#include "Terrain.h"
#include "Cal3dStore.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/stock.h>

#include <sigc++/object_slot.h>

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

    Gtk::ScrolledWindow * scrolled_window = manage( new Gtk::ScrolledWindow() );
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    get_vbox()->pack_start(*scrolled_window);

    Gtk::Button * b = add_button(Gtk::Stock::CANCEL , Gtk::RESPONSE_CANCEL);
    b->signal_clicked().connect(SigC::slot(*this, &NewLayerWindow::cancel));
    b = add_button(Gtk::Stock::OK , Gtk::RESPONSE_OK);
    b->signal_clicked().connect(SigC::slot(*this, &NewLayerWindow::okay));
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

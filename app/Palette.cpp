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

#include "Palette.h"

#include "MainWindow.h"
#include "Model.h"

#include <gtkmm/menu.h>
#include <gtkmm/notebook.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>

#include <iostream>
#include <sstream>
#include <vector>

Palette::Palette(MainWindow & mw) : OptionBox("Palette"),
                                    m_currentModel(0),
                                    m_mainWindow(mw)
{
    // Gtk::VBox * vbox = manage( new Gtk::VBox() );
    Gtk::VBox * vbox = this;

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") )), Gtk::PACK_SHRINK, 2);
    m_modelMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_modelMenu, Gtk::PACK_EXPAND_WIDGET, 2);
   
    vbox->pack_start(*tophbox, Gtk::PACK_SHRINK, 2);

    m_notebook = manage( new Gtk::Notebook() );
    m_notebook->set_tab_pos(Gtk::POS_TOP);

    //
    // static const gchar *titles[] = { "Name", "Image", NULL };
    // m_tile_clist = manage( new Gtk::CList(titles) );
    // m_tile_clist->set_column_width(0, 200);
    // m_tile_clist->set_column_width(1, 50);
    // m_tile_clist->select_row.connect(sigc::mem_fun(*this, &Palette::setCurrentTile));

    m_columns = new Gtk::TreeModelColumnRecord();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_visColumn = new Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> >();
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_visColumn);

    m_tileTreeModel = Gtk::ListStore::create(*m_columns);
    m_entityTreeModel = Gtk::ListStore::create(*m_columns);
    m_textureTreeModel = Gtk::ListStore::create(*m_columns);

    Gtk::TreeModel::Row tirow = *(m_tileTreeModel->append());
    tirow[*m_nameColumn] = Glib::ustring("test tile name");
    Gtk::TreeModel::Row enrow = *(m_entityTreeModel->append());
    enrow[*m_nameColumn] = Glib::ustring("test entity name");
    Gtk::TreeModel::Row terow = *(m_textureTreeModel->append());
    terow[*m_nameColumn] = Glib::ustring("test texture name");

    m_tileTreeView = manage( new Gtk::TreeView() );
    m_tileTreeView->set_model( m_tileTreeModel );
    m_tileTreeView->append_column("Name", *m_nameColumn);
    m_tileTreeView->append_column("Image", *m_visColumn);

    m_refTileTreeSelection = m_tileTreeView->get_selection();
    m_refTileTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    m_refTileTreeSelection->signal_changed().connect( sigc::mem_fun(*this, &Palette::setCurrentTile) );

    m_entityTreeView = manage( new Gtk::TreeView() );
    m_entityTreeView->set_model( m_entityTreeModel );
    m_entityTreeView->append_column("Name", *m_nameColumn);
    m_entityTreeView->append_column("Image", *m_visColumn);

    m_refEntityTreeSelection = m_entityTreeView->get_selection();
    m_refEntityTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    m_refEntityTreeSelection->signal_changed().connect( sigc::mem_fun(*this, &Palette::setCurrentEntity) );

    m_textureTreeView = manage( new Gtk::TreeView() );
    m_textureTreeView->set_model( m_textureTreeModel );
    m_textureTreeView->append_column("Name", *m_nameColumn);
    m_textureTreeView->append_column("Image", *m_visColumn);

    m_refTextureTreeSelection = m_textureTreeView->get_selection();
    m_refTextureTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    m_refTextureTreeSelection->signal_changed().connect( sigc::mem_fun(*this, &Palette::setCurrentTexture) );

    Gtk::ScrolledWindow * sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    sw->set_size_request(300,150);
    sw->add(*m_tileTreeView);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Tiles"));

    // static const gchar *entity_titles[] = { "Name", "Image", NULL };
    // m_entity_clist = manage( new Gtk::CList(entity_titles) );
    // m_entity_clist->set_column_width(0, 200);
    // m_entity_clist->set_column_width(1, 50);
    // m_entity_clist->select_row.connect(sigc::mem_fun(*this, &Palette::setCurrentEntity));
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    sw->set_size_request(300,150);
    sw->add(*m_entityTreeView);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Entities"));

    // static const gchar *texture_titles[] = { "Name", "Image", NULL };
    // m_texture_clist = manage( new Gtk::CList(texture_titles) );
    // m_texture_clist->set_column_width(0, 200);
    // m_texture_clist->set_column_width(1, 50);
    // m_texture_clist->select_row.connect(sigc::mem_fun(*this, &Palette::setCurrentTexture));
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    sw->set_size_request(300,200);
    sw->add(*m_textureTreeView);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Textures"));

    vbox->pack_start(*m_notebook, Gtk::PACK_EXPAND_WIDGET, 2);

    // add(*vbox);
    // set_title("Palette");
    set_sensitive(false);

    mw.modelAdded.connect(sigc::mem_fun(*this, &Palette::addModel));
    mw.currentModelChanged.connect(sigc::mem_fun(*this, &Palette::setModel));
    signal_delete_event().connect(sigc::mem_fun(*this, &Palette::deleteEvent));
}

void Palette::setCurrentTile()
{
    Gtk::TreeIter i = m_refTileTreeSelection->get_selected();
    if (!i) {
        return;
    }
    Gtk::TreeModel::Row row = *i;
    Glib::ustring t = row[*m_nameColumn];
    m_currentTile = t;
}

void Palette::setCurrentEntity()
{
    Gtk::TreeIter i = m_refEntityTreeSelection->get_selected();
    if (!i) {
        return;
    }
    Gtk::TreeModel::Row row = *i;
    Glib::ustring t = row[*m_nameColumn];
    m_currentEntity = t;
}

void Palette::setCurrentTexture()
{
    Gtk::TreeIter i = m_refTextureTreeSelection->get_selected();
    if (!i) {
        return;
    }
    Gtk::TreeModel::Row row = *i;
    Glib::ustring t = row[*m_nameColumn];
    m_currentTexture = t;
}

void Palette::addModel(Model * model)
{
    if (m_tiles.find(model) != m_tiles.end()) {
        return;
    }
    Gtk::Menu * menu = m_modelMenu->get_menu();
    std::stringstream ident;
    ident << model->getName() << "-" << model->getModelNo();
    m_tiles[model] = std::list<std::string>();
    m_entities[model] = std::list<std::string>();
    m_textures[model] = std::list<std::string>();
    if (menu == NULL) {
        menu = manage( new Gtk::Menu() );
        
        Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), sigc::bind<Model*>(sigc::mem_fun(*this, &Palette::setModel),model)));
        m_modelMenu->set_menu(*menu);
        set_sensitive(true);
        setModel(model);
    } else {
        Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), sigc::bind<Model*>(sigc::mem_fun(*this, &Palette::setModel),model)));
    }
}

void Palette::setModel(Model * model)
{
    m_typeMonitor.disconnect();

    if (model == NULL) {
        set_sensitive(false);
        // Clear all the pallettes
        return;
    }
    syncModel(model);

    m_typeMonitor = model->typesAdded.connect(sigc::bind(sigc::mem_fun(*this, &Palette::syncModel), model));
}

void Palette::syncModel(Model * model)
{
    std::cout << "SYINCING" << std::endl << std::flush;
    std::map<Model*,std::list<std::string> >::const_iterator I;

    I = m_tiles.find(model);
    if (I != m_tiles.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        // FIXME Clear the paletter windows
        m_tileTreeModel->clear();
        for (; J != entries.end(); J++) {
            Gtk::TreeModel::Row row = *(m_tileTreeModel->append());
            row[*m_nameColumn] = *J;
            // std::vector<std::string> entry(1, *J);
            // m_tile_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO TILES" << std::endl << std::flush; }
    Gtk::TreeModel::Row row = *(m_tileTreeModel->append());
    row[*m_nameColumn] = "TILE";

    I = m_entities.find(model);
    if (I != m_entities.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        // FIXME Clear the paletter windows
        m_entityTreeModel->clear();
        for (; J != entries.end(); J++) {
            Gtk::TreeModel::Row row = *(m_entityTreeModel->append());
            row[*m_nameColumn] = *J;
            // std::vector<std::string> entry(1, *J);
            // m_entity_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO ENTITIES" << std::endl << std::flush; }

    I = m_textures.find(model);
    if (I != m_textures.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        // FIXME Clear the paletter windows
        m_textureTreeModel->clear();
        for (; J != entries.end(); J++) {
            Gtk::TreeModel::Row row = *(m_textureTreeModel->append());
            row[*m_nameColumn] = *J;
            // std::vector<std::string> entry(1, *J);
            // m_texture_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO TEXTURES" << std::endl << std::flush; }

    // Update the contents of the windows to the new thingy
    set_sensitive(true);
}

void Palette::addTileEntry(Model * model, const std::string & name)
{
    std::map<Model*,std::list<std::string> >::iterator I = m_tiles.find(model);
    if (I == m_tiles.end()) {
        std::cerr << "ERROR: Adding palette tile entry for model that does not exist" << std::endl << std::flush;
    }

    std::list<std::string> & entries = I->second;

    entries.push_back(name);
}

void Palette::addEntityEntry(Model * model, const std::string & name)
{
    std::map<Model*,std::list<std::string> >::iterator I = m_entities.find(model);
    if (I == m_entities.end()) {
        std::cerr << "ERROR: Adding palette entitie entry for model that does not exist" << std::endl << std::flush;
    }

    std::list<std::string> & entries = I->second;

    entries.push_back(name);
}

void Palette::addTextureEntry(Model * model, const std::string & name)
{
    std::map<Model*,std::list<std::string> >::iterator I = m_textures.find(model);
    if (I == m_textures.end()) {
        std::cerr << "ERROR: Adding palette texture entry for model that does not exist" << std::endl << std::flush;
    }

    std::list<std::string> & entries = I->second;

    entries.push_back(name);
}

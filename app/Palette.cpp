// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Palette.h"

#include "MainWindow.h"
#include "Model.h"

#include <gtk--/notebook.h>
#include <gtk--/box.h>
#include <gtk--/clist.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/optionmenu.h>

#include <iostream>
#include <sstream>
#include <vector>

Palette::Palette(MainWindow & mw) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                    m_currentModel(NULL),
                                    m_mainWindow(mw)
{
    Gtk::VBox * vbox = manage( new Gtk::VBox() );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") )), false, false, 2);
    m_modelMenu = manage( new Gtk::OptionMenu() );
    tophbox->pack_start(*m_modelMenu, true, true, 2);
   
    vbox->pack_start(*tophbox, false, false, 2);

    m_notebook = manage( new Gtk::Notebook() );
    m_notebook->set_tab_pos(GTK_POS_TOP);

    //
    static const gchar *titles[] = { "Name", "Image", NULL };
    m_tile_clist = manage( new Gtk::CList(titles) );
    m_tile_clist->set_column_width(0, 200);
    m_tile_clist->set_column_width(1, 50);
    m_tile_clist->select_row.connect(SigC::slot(this, &Palette::setCurrentTile));
    Gtk::ScrolledWindow * sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,150);
    sw->add(*m_tile_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Tiles"));

    static const gchar *entity_titles[] = { "Name", "Image", NULL };
    m_entity_clist = manage( new Gtk::CList(entity_titles) );
    m_entity_clist->set_column_width(0, 200);
    m_entity_clist->set_column_width(1, 50);
    m_entity_clist->select_row.connect(SigC::slot(this, &Palette::setCurrentEntity));
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,150);
    sw->add(*m_entity_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Entities"));

    static const gchar *texture_titles[] = { "Name", "Image", NULL };
    m_texture_clist = manage( new Gtk::CList(texture_titles) );
    m_texture_clist->set_column_width(0, 200);
    m_texture_clist->set_column_width(1, 50);
    m_texture_clist->select_row.connect(SigC::slot(this, &Palette::setCurrentTexture));
    sw = manage( new Gtk::ScrolledWindow() );
    sw->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    sw->set_usize(300,200);
    sw->add(*m_texture_clist);
    m_notebook->pages().push_back(Gtk::Notebook_Helpers::TabElem(*sw,"Textures"));

    vbox->pack_start(*m_notebook, true, true, 2);

    add(*vbox);
    set_sensitive(false);
    set_title("Palette");

    mw.modelAdded.connect(SigC::slot(this, &Palette::addModel));
    mw.currentModelChanged.connect(SigC::slot(this, &Palette::setModel));
}

void Palette::setCurrentTile(int row, int column, GdkEvent *)
{
    m_currentTile = m_tile_clist->cell(row,0).get_text();
}

void Palette::setCurrentEntity(int row, int column, GdkEvent *)
{
    m_currentEntity = m_entity_clist->cell(row,0).get_text();
}

void Palette::setCurrentTexture(int row, int column, GdkEvent *)
{
    m_currentTexture = m_texture_clist->cell(row,0).get_text();
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
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(slot(this, &Palette::setModel),model)));
        m_modelMenu->set_menu(menu);
        set_sensitive(true);
        setModel(model);
    } else {
        Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(slot(this, &Palette::setModel),model)));
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

    m_typeMonitor = model->typesAdded.connect(bind(SigC::slot(this, &Palette::syncModel), model));
}

void Palette::syncModel(Model * model)
{
    std::map<Model*,std::list<std::string> >::const_iterator I;

    I = m_tiles.find(model);
    if (I != m_tiles.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        m_tile_clist->clear();
        for (; J != entries.end(); J++) {
            std::vector<Gtk::string> entry(1, *J);
            m_tile_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO TILES" << std::endl << std::flush; }

    I = m_entities.find(model);
    if (I != m_entities.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        m_entity_clist->clear();
        for (; J != entries.end(); J++) {
            std::vector<Gtk::string> entry(1, *J);
            m_entity_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO TILES" << std::endl << std::flush; }

    I = m_textures.find(model);
    if (I != m_textures.end()) {
        const std::list<std::string> & entries = I->second;
        std::list<std::string>::const_iterator J = entries.begin();
        m_texture_clist->clear();
        for (; J != entries.end(); J++) {
            std::vector<Gtk::string> entry(1, *J);
            m_texture_clist->rows().push_back(entry);
        }
    } else { std::cerr << "NO TILES" << std::endl << std::flush; }

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

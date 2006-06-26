// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_PALETTE_H
#define EQUATOR_APP_PALETTE_H

#include "gui/gtkmm/OptionBox.h"

#include <map>
#include <list>
#include <string>

class MainWindow;
class Model;

namespace Gtk {
   class Notebook;
   class OptionMenu;
   class ListStore;
   template <class T> class TreeModelColumn;
   class TreeModelColumnRecord;
   class TreeView;
   class TreeSelection;
}

class Palette : public OptionBox
{
  private:
    Gtk::Notebook * m_notebook;
    Glib::RefPtr<Gtk::ListStore> m_tileTreeModel;
    Glib::RefPtr<Gtk::ListStore> m_entityTreeModel;
    Glib::RefPtr<Gtk::ListStore> m_textureTreeModel;
    Gtk::TreeModelColumn<Glib::ustring> * m_nameColumn;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > * m_visColumn;
    Gtk::TreeModelColumnRecord * m_columns;
    Gtk::TreeView * m_tileTreeView;
    Gtk::TreeView * m_entityTreeView;
    Gtk::TreeView * m_textureTreeView;
    Glib::RefPtr<Gtk::TreeSelection> m_refTileTreeSelection;
    Glib::RefPtr<Gtk::TreeSelection> m_refEntityTreeSelection;
    Glib::RefPtr<Gtk::TreeSelection> m_refTextureTreeSelection;
    // Gtk::CList * m_tile_clist;
    // Gtk::CList * m_entity_clist;
    // Gtk::CList * m_texture_clist;
    Gtk::OptionMenu * m_modelMenu;
    Model * m_currentModel;
    sigc::connection m_typeMonitor;

    std::map<Model *, std::list<std::string> > m_tiles;
    std::map<Model *, std::list<std::string> > m_entities;
    std::map<Model *, std::list<std::string> > m_textures;

    std::string m_currentTile;
    std::string m_currentEntity;
    std::string m_currentTexture;

    void setCurrentTile();
    void setCurrentEntity();
    void setCurrentTexture();
  public:
    MainWindow & m_mainWindow;

    explicit Palette(MainWindow &);

    bool deleteEvent(GdkEventAny*) {
        hide();
        return 1;
    }

    void addModel(Model *);
    void setModel(Model *);
    void syncModel(Model *);

    void addTileEntry(Model *, const std::string &);
    void addEntityEntry(Model *, const std::string &);
    void addTextureEntry(Model *, const std::string &);

    const std::string & getCurrentTile() const {
        return m_currentTile;
    }

    const std::string & getCurrentEntity() const {
        return m_currentEntity;
    }

    const std::string & getCurrentTexture() const {
        return m_currentTexture;
    }

};

#endif // EQUATOR_APP_PALETTE_H

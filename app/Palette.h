// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_PALETTE_H
#define EQUATOR_APP_PALETTE_H

#include <gtk--/window.h>

#include <map>
#include <list>
#include <string>

class MainWindow;
class Model;

namespace Gtk {
  class Notebook;
  class CList;
  class OptionMenu;
}

class Palette : public Gtk::Window
{
  private:
    Gtk::Notebook * m_notebook;
    Gtk::CList * m_tile_clist;
    Gtk::CList * m_entity_clist;
    Gtk::CList * m_texture_clist;
    Gtk::OptionMenu * m_modelMenu;
    Model * m_currentModel;
    Gtk::Connection m_typeMonitor;

    std::map<Model *, std::list<std::string> > m_tiles;
    std::map<Model *, std::list<std::string> > m_entities;
    std::map<Model *, std::list<std::string> > m_textures;
  public:
    MainWindow & m_mainWindow;

    explicit Palette(MainWindow &);

    void addModel(Model *);
    void setModel(Model *);
    void syncModel(Model *);

    void addTileEntry(Model *, const std::string &);
    void addEntityEntry(Model *, const std::string &);
    void addTextureEntry(Model *, const std::string &);
};

#endif // EQUATOR_APP_PALETTE_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_PALETTE_H
#define EQUATOR_APP_PALETTE_H

#include <gtk--/window.h>

class MainWindow;

namespace Gtk {
  class Notebook;
}

class Palette : public Gtk::Window
{
  private:
    Gtk::Notebook * m_notebook;

  public:
    MainWindow & m_mainWindow;

    explicit Palette(MainWindow &);
};

#endif // EQUATOR_APP_PALETTE_H

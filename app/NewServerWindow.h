// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_NEWLAYERWINDOW_H
#define EQUATOR_APP_NEWLAYERWINDOW_H

#include <gtk--/window.h>
#include <gtk--/list.h>

class NewLayerWindow : public Gtk::Window
{
  public:
    NewLayerWindow();

    void doshow();

    void okay();
    void cancel();
};

#endif // EQUATOR_APP_NEWLAYERWINDOW_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include <iostream>

#include "MainWindow.h"

#include <gtk--/main.h>

int main(int argc, char ** argv)
{
    Gtk::Main kit(argc, argv);

    MainWindow * window = new MainWindow();

    kit.run();
    return 0;
}

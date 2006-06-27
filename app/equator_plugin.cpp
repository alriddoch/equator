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

#include "MainWindow.h"

#include "common/configuration.h"

#include <gtkmm.h>

#include <gtkglmm.h>

#include <iostream>

Gtk::Main * kit;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef BUILD_AS_GIMP_PLUGIN

#include <libgimp/gimp.h>
#include <libgimp/gimpintl.h>

extern "C" void query(void);
extern "C" void run(gchar * name,
                    gint nparams,
                    GimpParam *param,
                    gint *nreturn_vals,
                    GimpParam **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
    NULL,  /* init_proc  */
    NULL,  /* quit_proc  */
    query, /* query_proc */
    run,   /* run_proc   */
};

extern "C" void query(void)
{
    static GimpParamDef args[] =
    {
        { GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive" },
    };

    static gint nargs = sizeof (args) / sizeof (args[0]);

    gimp_install_procedure("extension_worldforge_equator",
                           "edit WorldForge server",
                           "I don't know what this string should say",
                           "Al Riddoch <alriddoch@zepler.org>",
                           "Al Riddoch",
                           "2001",
                           N_("<Toolbox>/Xtns/WorldForge/Equator..."),
                           NULL,
                           GIMP_EXTENSION,
                           nargs, 0, args, NULL);
}

extern "C" void run(gchar * name,
                    gint nparams,
                    GimpParam *param,
                    gint *nreturn_vals,
                    GimpParam **return_vals)
{

    MainWindow * window = new MainWindow();

    kit->run();
    return;
}

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);

    kit = new Gtk::Main(argc, argv);

    Gdk::GL::init();

    int type = gimp_main (argc, argv);

    if (type == 1) { // We are not being invoked as a plugin
        std::cout << "Dropping back to running as an application instead"
                  << std::endl << std::flush;
        MainWindow * window = new MainWindow();

        kit->run();
    }
    return 0;
}

#else // BUILD_AS_GIMP_PLUGIN

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);

    kit = new Gtk::Main(argc, argv);

    Gdk::GL::init(argc, argv);

    /* MainWindow * window = */ new MainWindow();

    kit->run();

    return 0;
}
#endif // BUILD_AS_GIMP_PLUGIN

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include "MainWindow.h"

#include <gtk--/main.h>

Gtk::Main * kit;

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

    window->set_title("Equator");

    kit->run();
    return;
}

int main(int argc, char ** argv)
{
    kit = new Gtk::Main(argc, argv);

    int type = gimp_main (argc, argv);

    if (type == 1) { // We are not being invoked as a plugin
        MainWindow * window = new MainWindow();

        window->set_title("Equator");

        kit->run();
    }
    return 0;
}

#else // BUILD_AS_GIMP_PLUGIN

int main(int argc, char ** argv)
{
    kit = new Gtk::Main(argc, argv);

    MainWindow * window = new MainWindow();

    window->set_title("Equator");

    kit->run();

    return 0;
}
#endif // BUILD_AS_GIMP_PLUGIN

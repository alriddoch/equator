/* Hello World (Gnome Edition)
 * Listing 1
 * This code is public domain, so use it as you please.
 */

/* this include will include everything needed for GNOME, including all the
 * libraries that gnome needs, such as gtk, imlib, etc ...*/
#include <gnome.h>
/* this is usually defined by autoconf but we're just using simple makefiles */
#define VERSION "1.0"

/* "callback" function (signal handler) which will quit the application*/
static void
exit_hello(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

/* callback function for when the window closes */
static int
delete_event(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
    return FALSE; /* false means continue with closing the window */
}

/* a callback for the about menu item, it will display a simple "About"
 * dialog box standard to all gnome applications
 */
static void
about_hello(GtkWidget *widget, gpointer data)
{
    GtkWidget *box;
    const char *authors[] = {
        "James Bond",
        NULL
    };

    box = gnome_about_new(/*title: */ "Hello World (Gnome Edition)",
                        /*version: */VERSION,
                        /*copyright: */ "(C) 1999 Secret Agents Inc.",
                        /*authors: */authors,
                        /*other comments: */
                        "An extremely complicated application which "
                        "does absolutely nothing useful",
                        NULL);
    gtk_widget_show(box);
}

static void file_new(GtkWidget * wdiget, gpointer data)
{

}

/* define the menus here */

static GnomeUIInfo file_menu [] = {
    GNOMEUIINFO_MENU_NEW_ITEM("New View", "Create a new View window", file_new, NULL),
    /* some item which is not one of the standard ones, the null
     * would be the callback, however we don't want to really do anything */
    GNOMEUIINFO_ITEM_NONE("Something","Just an item which does nothing",NULL),
    /* standard exit item */
    GNOMEUIINFO_MENU_EXIT_ITEM(exit_hello,NULL),
    GNOMEUIINFO_END
};

static GnomeUIInfo help_menu [] = {
    /* load the helpfiles for this application if available */
    //GNOMEUIINFO_HELP("hello_world"),
    /* the standard about item */
    GNOMEUIINFO_MENU_ABOUT_ITEM(about_hello,NULL),
    GNOMEUIINFO_END
};

/* define the main menubar */
static GnomeUIInfo main_menu [] = {
    GNOMEUIINFO_MENU_FILE_TREE(file_menu),
    GNOMEUIINFO_MENU_HELP_TREE(help_menu),
    GNOMEUIINFO_END
};

/* Our main function */
int
main(int argc, char *argv[])
{
    GtkWidget *mdi; /* pointer to our main window */
    GtkWidget *w; /* pointer to some widget */

    /* initialize gnome */
    gnome_init("hello_world", VERSION, argc, argv);

    /* create main window */
    mdi = (GtkWidget*) gnome_mdi_new("hello_world", "Hello World (Gnome Edition)");

    gnome_mdi_set_mode(GNOME_MDI(mdi), GNOME_MDI_TOPLEVEL);

    gnome_mdi_set_menubar_template(GNOME_MDI(mdi), main_menu);

    gnome_mdi_open_toplevel(GNOME_MDI(mdi));

    /* connect "delete_event" (happens when the window is closed */
    // gtk_signal_connect(GTK_OBJECT(mdi), "delete_event",
    // GTK_SIGNAL_FUNC(delete_event),
    // NULL);

    /* add the menus to the main window */
    //gnome_app_create_menus(GNOME_APP(app), main_menu);

    /* setup appbar (bottom of window bar for status, menu hints and
    * progress display) */
    //w = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_USER);
    //gnome_app_set_statusbar(GNOME_APP(app), w);

    /* make menu hints display on the appbar */
    //gnome_app_install_menu_hints(GNOME_APP(app), main_menu);

    /* set up some bogus window contents */
    //w = gtk_label_new("Some really really really random\ntext\ngoes\nhere!");

    /* add those contents to the window */
    //gnome_app_set_contents(GNOME_APP(app), w);

    /* show all the widgets on the main window and the window itself */
    gtk_widget_show_all(mdi);

    /* run the main loop */
    gtk_main();

    return 0;
}


// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "LayerWindow.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "GlView.h"
#include "Layer.h"
#include "NewLayerWindow.h"

#include <gtk--/frame.h>
#include <gtk--/menuitem.h>
#include <gtk--/box.h>
#include <gtk--/clist.h>
#include <gtk--/label.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/button.h>
#include <gtk--/separator.h>
#include <gtk--/optionmenu.h>

#include <iostream>
#include <vector>

LayerWindow::LayerWindow(MainWindow & w) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                           m_currentView(NULL), m_mainWindow(w)
{
    // destroy.connect(slot(this, &LayerWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("View:") ) ), false, false, 2);
    m_viewMenu = manage( new Gtk::OptionMenu() );

    Gtk::Menu * menu = manage( new Gtk::Menu() );

    m_viewMenu->set_menu(menu);
    tophbox->pack_start(*m_viewMenu, true, true, 2);
    tophbox->pack_end(*(manage( new Gtk::Label("WOOT") ) ), false, false, 2);
   
    vbox->pack_start(*tophbox, false, false, 2);
    
    static const gchar *titles[] = { "Visible", "Type", "Name", NULL };
    m_clist = manage( new Gtk::CList(titles) );
    //clist->column_titles_hide();
    m_clist->set_column_width (0, 50);
    m_clist->set_column_width (1, 80);
    m_clist->set_column_width (2, 80);

    vbox->pack_start(*manage(new Gtk::HSeparator()), false, false, 0);

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    scrolled_window->add(*m_clist);

    vbox->pack_start(*scrolled_window, true, true, 2);

    Gtk::HBox * bothbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button("New...") );
    b->clicked.connect(slot(this, &LayerWindow::newLayerRequested));
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button("Raise") );
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button("Lower") );
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button("Duplicate") );
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button("Delete") );
    bothbox->pack_start(*b, true, true, 0);

    vbox->pack_end(*bothbox, false, true, 2);

    add(*vbox);
    set_title("Layers");
    set_sensitive(false);

    m_newLayerWindow = manage( new NewLayerWindow() );
    // show_all();

    w.modelAdded.connect(SigC::slot(this, &LayerWindow::addModel));

}

void LayerWindow::setView(GlView * view)
{
    m_clist->clear();

    m_currentView = view;

    if (m_currentView == NULL) {
        set_sensitive(false);
        return;
    }

    set_sensitive(true);

    const std::list<Layer *> & layers = view->getLayers();
 
    std::list<Layer *>::const_iterator I = layers.begin();
    for (int i = 0; I != layers.end(); I++,i++) {
        std::vector<Gtk::string> entry(1, (*I)->isVisible() ? "X" : "");
        entry.push_back((*I)->getType());
        entry.push_back((*I)->getName());
        m_clist->rows().push_back(entry);
        if (i == view->getCurrentLayer()) {
            m_clist->rows().back()->select();
        }
    }

    cout << "Finished adding to list" << std::endl << std::flush;
}

void LayerWindow::newLayerRequested()
{
    m_newLayerWindow->doshow(m_currentView);
}

void LayerWindow::addModel(ViewWindow * view)
{
    cout << "LayerWindow notified of new model" << std::endl << std::flush;
    Gtk::Menu_Helpers::MenuList& model_menu = m_viewMenu->get_menu()->items();
    if (model_menu.empty()) {
        set_sensitive(true);
        setView(view->getView());
    }
    model_menu.push_back(Gtk::Menu_Helpers::MenuElem(view->getName(), SigC::bind<GlView*>(slot(this, &LayerWindow::setView),view->getView())));
}

// FIXME How do we get notification of the current view?
// FIXME How do we get notification of changes in layers?

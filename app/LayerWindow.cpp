// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "LayerWindow.h"
#include "MainWindow.h"
#include "Model.h"
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
#include <gtk--/pixmap.h>

#include <iostream>
#include <vector>
#include <list>
#include <sstream>

#include "../eye.xpm"
#include "../null.xpm"

LayerWindow::LayerWindow(MainWindow & mw) : Gtk::Window(GTK_WINDOW_TOPLEVEL),
                                            m_currentModel(NULL)
{
    // destroy.connect(slot(this, &LayerWindow::destroy_handler));
    Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") )), false, false, 2);
    m_modelMenu = manage( new Gtk::OptionMenu() );

    tophbox->pack_start(*m_modelMenu, true, true, 2);
    tophbox->pack_end(*(manage( new Gtk::Label("WOOT") ) ), false, false, 2);
   
    vbox->pack_start(*tophbox, false, false, 2);
    
    static const gchar *titles[] = { "Visible", "Type", "Name", NULL };
    m_clist = manage( new Gtk::CList(titles) );

    m_clist->select_row.connect(slot(this,&LayerWindow::selectionMade));
    //clist->column_titles_hide();
    m_clist->set_column_width (0, 50);
    m_clist->set_column_width (1, 80);
    m_clist->set_column_width (2, 80);

    vbox->pack_start(*manage(new Gtk::HSeparator()), false, false, 0);

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    scrolled_window->set_usize(250,150);
    scrolled_window->add(*m_clist);

    vbox->pack_start(*scrolled_window, true, true, 0);

    Gtk::HBox * bothbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button() );
    Gtk::Pixmap * p = manage( new Gtk::Pixmap("newlayer.xpm") );
    b->add(*p);
    b->clicked.connect(slot(this, &LayerWindow::newLayerRequested));
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button() );
    p = manage( new Gtk::Pixmap("raise.xpm") );
    b->add(*p);
    b->clicked.connect(slot(this, &LayerWindow::raiseLayer));
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button() );
    p = manage( new Gtk::Pixmap("lower.xpm") );
    b->add(*p);
    b->clicked.connect(slot(this, &LayerWindow::lowerLayer));
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button() );
    p = manage( new Gtk::Pixmap("duplicate.xpm") );
    b->add(*p);
    bothbox->pack_start(*b, true, true, 0);
    b = manage( new Gtk::Button() );
    p = manage( new Gtk::Pixmap("delete.xpm") );
    b->add(*p);
    bothbox->pack_start(*b, true, true, 0);

    vbox->pack_end(*bothbox, false, true, 0);

    add(*vbox);
    set_title("Layers");
    set_sensitive(false);

    m_eye = gdk_pixmap_create_from_xpm_d(m_clist->gtkobj()->clist_window,
            &m_eyemask, &GTK_WIDGET(m_clist->gtkobj())->style->white, eye_xpm);

    m_null = gdk_pixmap_create_from_xpm_d(m_clist->gtkobj()->clist_window,
           &m_nullmask, &GTK_WIDGET(m_clist->gtkobj())->style->white, null_xpm);

    m_newLayerWindow = manage( new NewLayerWindow() );
    // show_all();

    mw.modelAdded.connect(SigC::slot(this, &LayerWindow::addModel));
    mw.currentModelChanged.connect(SigC::slot(this, &LayerWindow::setModel));

}

void LayerWindow::setModel(Model * model)
{
    if (m_currentModel != NULL) {
        m_updateSignal.disconnect();
    }

    m_currentModel = model;

    if (m_currentModel == NULL) {
        set_sensitive(false);
        m_clist->clear();
        return;
    }

    m_updateSignal = m_currentModel->layersChanged.connect(slot(this, &LayerWindow::updateLayers));
    updateLayers();

    set_sensitive(true);
}

void LayerWindow::updateLayers()
{

    m_clist->clear();
    const std::list<Layer *> & layers = m_currentModel->getLayers();
 
    std::list<Layer *>::const_iterator I = layers.begin();
    for (; I != layers.end(); I++) {
        std::vector<Gtk::string> entry(1,"");
        entry.push_back((*I)->getType());
        entry.push_back((*I)->getName());
        m_clist->rows().push_front(entry);
        if (*I == m_currentModel->getCurrentLayer()) {
            m_clist->rows().front()->select();
        }
        if (!(*I)->isVisible()) {
            continue;
        }
        m_clist->cell(0, 0).set_pixmap(m_eye,m_eyemask);
    }

}

void LayerWindow::addModel(Model * model)
{
    Gtk::Menu * menu = m_modelMenu->get_menu();
    std::stringstream ident;
    ident << model->getName() << "-" << model->getModelNo();
    if (menu == NULL) {
        menu = manage( new Gtk::Menu() );
        
        Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(slot(this, &LayerWindow::setModel),model)));
        m_modelMenu->set_menu(menu);
        set_sensitive(true);
        setModel(model);
    } else {
        Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
        model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(slot(this, &LayerWindow::setModel),model)));
    }
    // m_viewMenu->set_menu(m_viewMenu->get_menu());
}

void LayerWindow::selectionMade(gint row, gint column, GdkEvent * event)
{
    const std::list<Layer *> & layers = m_currentModel->getLayers();
    std::list<Layer *>::const_iterator I = layers.begin();
    for (int i = layers.size() - 1; i > row && I != layers.end(); --i, ++I) { }
    if (I == layers.end()) {
        std::cerr << "No layer described" << std::endl << std::flush;
        return;
    }
    m_currentModel->setCurrentLayer(*I);
    std::cout << "new sel " << row << " " << column << " " << (*I)->getName() << std::endl << std::flush;
    if (column == 0) {
        bool vis = (*I)->toggleVisible();
        if (vis) {
            m_clist->cell(row, column).set_pixmap(m_eye,m_eyemask);
        } else {
            m_clist->cell(row, column).set_pixmap(m_null,m_nullmask);
        }
        m_currentModel->update();
    }
}

void LayerWindow::newLayerRequested()
{
    m_newLayerWindow->doshow(m_currentModel);
}

void LayerWindow::raiseLayer()
{
    m_currentModel->raiseCurrentLayer();
}

void LayerWindow::lowerLayer()
{
    m_currentModel->lowerCurrentLayer();
}

// FIXME How do we get notification of the current view?
// FIXME How do we get notification of changes in layers?

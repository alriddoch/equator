// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "LayerWindow.h"
#include "MainWindow.h"
#include "Model.h"
#include "Layer.h"
#include "NewLayerWindow.h"

#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treeselection.h>

#include <iostream>
#include <vector>
#include <list>
#include <sstream>

// #include "../eye.xpm"
// #include "../null.xpm"
#include "../newlayer.xpm"
#include "../raise.xpm"
#include "../lower.xpm"
#include "../duplicate.xpm"
#include "../delete.xpm"

LayerWindow::LayerWindow(MainWindow & mw) : OptionBox("Layers"),
                                            m_currentModel(0)
{
    // destroy.connect(slot(this, &LayerWindow::destroy_handler));
    // Gtk::VBox * vbox = manage( new Gtk::VBox(false, 2) );
    Gtk::VBox * vbox = this;

    Gtk::HBox * tophbox = manage( new Gtk::HBox() );

    tophbox->pack_start(*(manage( new Gtk::Label("Model:") )), Gtk::PACK_SHRINK, 2);
    m_modelMenu = manage( new Gtk::OptionMenu() );

    tophbox->pack_start(*m_modelMenu, Gtk::PACK_EXPAND_WIDGET, 2);
    tophbox->pack_start(*(manage( new Gtk::Label("WOOT") ) ), Gtk::PACK_SHRINK, 2);
   
    vbox->pack_start(*tophbox, Gtk::PACK_SHRINK, 2);
    
    m_columns = new Gtk::TreeModelColumnRecord();
    m_visColumn = new Gtk::TreeModelColumn<bool>();
    m_typeColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_nameColumn = new Gtk::TreeModelColumn<Glib::ustring>();
    m_ptrColumn = new Gtk::TreeModelColumn<Layer*>();
    m_columns->add(*m_visColumn);
    m_columns->add(*m_typeColumn);
    m_columns->add(*m_nameColumn);
    m_columns->add(*m_ptrColumn);

    m_treeModel = Gtk::ListStore::create(*m_columns);

    m_treeView = manage( new Gtk::TreeView() );

    m_treeView->set_model( m_treeModel );

    Gtk::CellRendererToggle * crt = manage( new Gtk::CellRendererToggle() );
    crt->signal_toggled().connect( SigC::slot(*this, &LayerWindow::visibleToggled) );
    int column_no = m_treeView->append_column("Visible", *crt);
    Gtk::TreeViewColumn * column = m_treeView->get_column(column_no - 1);
    column->add_attribute(crt->property_active(), *m_visColumn);
    column->set_clickable();

    m_treeView->append_column("Type", *m_typeColumn);
    m_treeView->append_column("Name", *m_nameColumn);

    m_refTreeSelection = m_treeView->get_selection();
    m_refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
    m_refTreeSelection->signal_changed().connect( SigC::slot(*this, &LayerWindow::selectionChanged) );

    vbox->pack_start(*manage(new Gtk::HSeparator()), Gtk::PACK_SHRINK);

    Gtk::ScrolledWindow *scrolled_window = manage(new Gtk::ScrolledWindow());
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    scrolled_window->set_size_request(250,150);
    scrolled_window->add(*m_treeView);

    vbox->pack_start(*scrolled_window);

    Gtk::HBox * bothbox = manage( new Gtk::HBox() );
    Gtk::Button * b = manage( new Gtk::Button() );


    Glib::RefPtr<Gdk::Bitmap> pixmask;
    Glib::RefPtr<Gdk::Pixmap> p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, newlayer_xpm);
    b->add_pixmap(p, pixmask);

    b->signal_clicked().connect(slot(*this, &LayerWindow::newLayerRequested));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    b = manage( new Gtk::Button() );
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, raise_xpm);
    b->add_pixmap(p, pixmask);
    b->signal_clicked().connect(slot(*this, &LayerWindow::raiseLayer));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    b = manage( new Gtk::Button() );
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, lower_xpm);
    b->add_pixmap(p, pixmask);
    b->signal_clicked().connect(slot(*this, &LayerWindow::lowerLayer));
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    b = manage( new Gtk::Button() );
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, duplicate_xpm);
    b->add_pixmap(p, pixmask);
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    b = manage( new Gtk::Button() );
    p = Gdk::Pixmap::create_from_xpm(get_colormap(), pixmask, delete_xpm);
    b->add_pixmap(p, pixmask);
    bothbox->pack_start(*b, Gtk::PACK_EXPAND_PADDING, 6);

    vbox->pack_start(*bothbox, Gtk::PACK_SHRINK, 6);

    // add(*vbox);
    // set_title("Layers");
    set_sensitive(false);

    // FIXME Organise the xpms
    //m_eye = gdk_pixmap_create_from_xpm_d(m_clist->gtkobj()->clist_window,
            //&m_eyemask, &GTK_WIDGET(m_clist->gtkobj())->style->white, eye_xpm);

    //m_null = gdk_pixmap_create_from_xpm_d(m_clist->gtkobj()->clist_window,
           //&m_nullmask, &GTK_WIDGET(m_clist->gtkobj())->style->white, null_xpm);

    m_newLayerWindow = new NewLayerWindow();
    // show_all();

    mw.modelAdded.connect(SigC::slot(*this, &LayerWindow::modelAdded));
    mw.currentModelChanged.connect(SigC::slot(*this, &LayerWindow::currentModelChanged));

    signal_delete_event().connect(slot(*this, &LayerWindow::deleteEvent));
}

void LayerWindow::currentModelChanged(Model * model)
{
    if (model == m_currentModel) {
        return;
    }

    if (m_currentModel != NULL) {
        m_updateSignal.disconnect();
    }

    m_currentModel = model;
    m_treeModel->clear();

    if (m_currentModel == NULL) {
        set_sensitive(false);
        return;
    }

    m_updateSignal = m_currentModel->layersChanged.connect(slot(*this, &LayerWindow::layersChanged));
    layersChanged();

    set_sensitive(true);
}

void LayerWindow::layersChanged()
{
    std::cout << "Got update layers" << std::endl << std::flush;
    m_treeModel->clear();
    const std::list<Layer *> & layers = m_currentModel->getLayers();
 
    std::list<Layer *>::const_iterator I = layers.begin();
    for (; I != layers.end(); I++) {
        Gtk::TreeModel::Row row = *(m_treeModel->append());
        row[*m_visColumn] = (*I)->isVisible();
        row[*m_typeColumn] = Glib::ustring((*I)->getType());
        row[*m_nameColumn] = Glib::ustring((*I)->getName());
        row[*m_ptrColumn] = *I;
        if (*I == m_currentModel->getCurrentLayer()) {
            m_refTreeSelection->select(row);
        }
    }
}

void LayerWindow::modelAdded(Model * model)
{
    std::cout << "Got add model" << std::endl << std::flush;
    Gtk::Menu * menu = m_modelMenu->get_menu();
    bool newMenu = false;

    if (menu == NULL) {
        newMenu = true;
        menu = manage( new Gtk::Menu() );
        m_modelMenu->set_menu(*menu);
        set_sensitive(true);
    }
    
    Gtk::Menu_Helpers::MenuList& model_menu = menu->items();
    std::stringstream ident;
    ident << model->getName() << "-" << model->getModelNo();
    model_menu.push_back(Gtk::Menu_Helpers::MenuElem(ident.str(), SigC::bind<Model*>(slot(*this, &LayerWindow::currentModelChanged),model)));
    if (newMenu) {
        m_modelMenu->set_history(0);
        currentModelChanged(model);
    }
}

void LayerWindow::visibleToggled(const Glib::ustring& path_string)
{
    std::cout << "GGO: " << path_string << std::endl << std::flush;
    GtkTreePath *gpath = gtk_tree_path_new_from_string (path_string.c_str());
    Gtk::TreePath path(gpath);

    /* get toggled iter */
    Gtk::TreeRow row = *(m_treeModel->get_iter(path));

    bool visible = row[*m_visColumn];
    Layer * layer = row[*m_ptrColumn];

    /* do something with the value */
    visible = layer->toggleVisible();

    /* set new value */
    row[*m_visColumn] = visible;

    if (0 != m_currentModel) {
        m_currentModel->updated.emit();
    }
}

void LayerWindow::selectionChanged()
{
    std::cout << "Got selection change" << std::endl << std::flush;
    Gtk::TreeIter i = m_refTreeSelection->get_selected();
    if (!i) {
        std::cout << "Nothing selected no more" << std::endl << std::flush;
        return;
    }
    Gtk::TreeModel::Row row = *i;
    Layer * layer = row[*m_ptrColumn];

    m_currentModel->setCurrentLayer(layer);
    m_currentModel->updated.emit();

    std::cout << layer->getName() << " is now current" << std::endl << std::flush;
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

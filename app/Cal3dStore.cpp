// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Model.h"

#include "Cal3dStore.h"
#include "MainWindow.h"

#include "gui/gtkmm/Cal3dStoreOptions.h"

#include <gtkmm/stock.h>
#include <gtkmm/fileselection.h>
#include <gtkmm/filechooserdialog.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <sys/types.h>
#include <sys/stat.h>

Cal3dStore::Cal3dStore(Model & model) :
                       Layer(model, "<empty>", "Cal3d animation")
{
    m_optionsBox = new Cal3dStoreOptions(*this);

    m_lastUpdate.assign_current_time();
}

void Cal3dStore::loadModel(const std::string & path)
{
    struct stat sbuf;
    if (stat(path.c_str(), &sbuf)) {
        // FIXME Message Dialog
        std::cerr << "Loading cal3d model " << path << " does not exist"
                  << std::endl << std::flush;
        return;
    }
    if (!m_cal3dModel.onInit(path)) {
        // FIXME Message Dialog
        std::cerr << "Loading cal3d model " << path << " failed"
                  << std::endl << std::flush;
        return;
    }

    unsigned int pos = path.find_last_of("/");
    if (pos != std::string::npos) {
        std::string file = path.substr(pos + 1, std::string::npos);
        m_name = file;
        m_model.layersChanged.emit();
    }

    m_cal3dModel.setLodLevel(1.f);
    m_cal3dModel.onUpdate(0.f);
}

void Cal3dStore::loadFile(const std::string & path)
{
    std::string suffix;
    unsigned int pos = path.find_last_of(".");

    if (pos != std::string::npos) {
        suffix = path.substr(pos + 1, std::string::npos);
    }

    if (suffix == "cfg") {
        loadModel(path);
    } else if (suffix == "cal") {
        std::cout << ".cal files not yet supported" << std::endl << std::flush;
    } else {
        // FIXME Message Dialog
        std::cout << "UNKNOWN" << std::endl << std::flush;
        // wuh
    }
}

void Cal3dStore::import_response(int response, Gtk::FileChooserDialog * fc)
{
    switch (response) {
        case Gtk::RESPONSE_OK:
            std::cout << "LOAD: " << fc->get_filename()
                      << std::endl << std::flush;
            loadFile(fc->get_filename());
            break;
        case Gtk::RESPONSE_CANCEL:
            break;
        default:
            break;
    }
    delete fc;
}

void Cal3dStore::options()
{
    std::cout << "Cal3dStore::options" << std::endl << std::flush;
    m_model.m_mainWindow.showOptionBox(*m_optionsBox);
}

void Cal3dStore::importFile()
{
    Gtk::FileChooserDialog * fc = new Gtk::FileChooserDialog("Import Cal3d File");
    // fc->set_transient_for(*this);

    fc->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    fc->add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);


    Gtk::FileFilter filter_cfg;
    filter_cfg.set_name("Cal3d Model files");
    filter_cfg.add_pattern("*.cfg");
    filter_cfg.add_pattern("*.cal");
    fc->add_filter(filter_cfg);

    Gtk::FileFilter filter_component;
    filter_component.set_name("Cal3d Component files");
    filter_component.add_pattern("*.caf");
    filter_component.add_pattern("*.cmf");
    filter_component.add_pattern("*.cmf");
    fc->add_filter(filter_component);

    fc->signal_response().connect(SigC::bind<Gtk::FileChooserDialog*>(SigC::slot(*this, &Cal3dStore::import_response), fc));
    fc->show_all();
}

void Cal3dStore::exportFile()
{
}

void Cal3dStore::selectInvert()
{
}

void Cal3dStore::selectAll()
{
}

void Cal3dStore::selectNone()
{
}

void Cal3dStore::draw(GlView & view)
{
    glEnable(GL_NORMALIZE);
    m_cal3dModel.onRender();
    glDisable(GL_NORMALIZE);
}

bool Cal3dStore::animate(GlView & view)
{
    Glib::TimeVal delta = m_model.m_mainWindow.time();
    delta.subtract(m_lastUpdate);
    std::cout << "Cal3dStore::animate" << delta.as_double() << std::endl << std::flush;
    m_lastUpdate = m_model.m_mainWindow.time();
    m_cal3dModel.onUpdate(delta.as_double());
    return true;
}

void Cal3dStore::select(GlView & view, int x, int y)
{
}

void Cal3dStore::select(GlView & view, int x, int y, int w, int h)
{
}

void Cal3dStore::pushSelection()
{
}

void Cal3dStore::popSelection()
{
}

void Cal3dStore::dragStart(GlView & view, int x, int y)
{
}

void Cal3dStore::dragUpdate(GlView & view, const WFMath::Vector<3> &)
{
}

void Cal3dStore::dragEnd(GlView & view, const WFMath::Vector<3> &)
{
}

void Cal3dStore::insert(const PosType &)
{
}

void Cal3dStore::align(Alignment )
{
}

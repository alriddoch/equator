// Equator Online RPG World Building Tool
// Copyright (C) 2007 Alistair Riddoch
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

#include "Model.h"

#include "Forest.h"
#include "MainWindow.h"

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

Forest::Forest(Model & model) :
                       Layer(model, "<empty>", "Mercator Forest")
{
    // m_optionsBox = new ForestOptions(*this);
}

void Forest::loadModel(const std::string & path)
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

void Forest::loadFile(const std::string & path)
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

void Forest::import_response(int response, Gtk::FileChooserDialog * fc)
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

void Forest::options()
{
    std::cout << "Forest::options" << std::endl << std::flush;
    m_model.m_mainWindow.showOptionBox(*m_optionsBox);
}

void Forest::importFile()
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

    fc->signal_response().connect(sigc::bind<Gtk::FileChooserDialog*>(sigc::mem_fun(*this, &Forest::import_response), fc));
    fc->show_all();
}

void Forest::exportFile()
{
}

void Forest::selectInvert()
{
}

void Forest::selectAll()
{
}

void Forest::selectNone()
{
}

void Forest::draw(GlView & view)
{
    glEnable(GL_NORMALIZE);
    m_cal3dModel.onRender();
    glDisable(GL_NORMALIZE);
}

bool Forest::animate(GlView & view)
{
    Glib::TimeVal delta = m_model.m_mainWindow.time();
    delta.subtract(m_lastUpdate);
    m_lastUpdate = m_model.m_mainWindow.time();
    m_cal3dModel.onUpdate(delta.as_double());
    return true;
}

void Forest::select(GlView & view, int x, int y)
{
}

void Forest::select(GlView & view, int x, int y, int w, int h)
{
}

void Forest::pushSelection()
{
}

void Forest::popSelection()
{
}

void Forest::dragStart(GlView & view, int x, int y)
{
}

void Forest::dragUpdate(GlView & view, const WFMath::Vector<3> &)
{
}

void Forest::dragEnd(GlView & view, const WFMath::Vector<3> &)
{
}

void Forest::insert(const PosType &)
{
}

void Forest::align(Alignment )
{
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Model.h"
#include "Cal3dStore.h"

#include <gtkmm/fileselection.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <sys/types.h>
#include <sys/stat.h>

Cal3dStore::Cal3dStore(Model & model) :
                       Layer(model, "<empty>", "Cal3d animation")
{
}

void Cal3dStore::load(Gtk::FileSelection * fsel)
{
    std::string path = fsel->get_filename();
    delete fsel;

    loadModel(path);
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

void Cal3dStore::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void Cal3dStore::options()
{
}

void Cal3dStore::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Cal3d Model");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(SigC::slot(*this, &Cal3dStore::load),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(SigC::slot(*this, &Cal3dStore::cancel),fsel));
    fsel->show();
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
    return false;
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

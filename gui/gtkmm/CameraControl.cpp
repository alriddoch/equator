// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2004 Alistair Riddoch

#include "CameraControl.h"

#include "app/Model.h"
#include "app/GlView.h"

#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/scrollbar.h>

CameraControl::CameraControl(GlView & v) : OptionBox(v.m_model.getName() + " Camera"),
                                           view(v)
{
    Gtk::VBox * vbox = this;

    // m_dAdjust = manage( new Gtk::Adjustment(view.getDeclination(), 0, 360) );
    // m_dAdjust->signal_value_changed().connect(slot(*this, &CameraControl::dChange));
    // m_rAdjust = manage( new Gtk::Adjustment(view.getDeclination(), 0, 360) );
    // m_rAdjust->signal_value_changed().connect(slot(*this, &CameraControl::rChange));

    vbox->pack_start(* manage( new Gtk::HScrollbar(v.getRotationAdjustment())));
    vbox->pack_start(* manage( new Gtk::HScrollbar(v.getDeclinationAdjustment())));
    vbox->pack_start(* manage( new Gtk::HScrollbar(v.getScaleAdjustment())));
}

void CameraControl::dChange()
{
    view.setDeclination(m_dAdjust->get_value());
    view.scheduleRedraw();
}

void CameraControl::rChange()
{
    view.setRotation(m_rAdjust->get_value());
    view.scheduleRedraw();
}

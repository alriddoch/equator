// Equator Online RPG World Building Tool
// Copyright (C) 2003-2004 Alistair Riddoch
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

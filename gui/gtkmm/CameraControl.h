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

#ifndef EQUATOR_GUI_GTKMM_CAMERA_CONTROL_H
#define EQUATOR_GUI_GTKMM_CAMERA_CONTROL_H

#include "OptionBox.h"

class GlView;

namespace Gtk {
  class Adjustment;
}

class CameraControl : public OptionBox
{
  private:
    Gtk::Adjustment * m_dAdjust;
    Gtk::Adjustment * m_rAdjust;
  public:
    GlView & view;

    CameraControl(GlView & v);

    void dChange();
    void rChange();
};

#endif // EQUATOR_GUI_GTKMM_CAMERA_CONTROL_H

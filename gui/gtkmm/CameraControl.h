// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2004 Alistair Riddoch

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

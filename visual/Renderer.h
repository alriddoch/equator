// Equator Online RPG World Building Tool
// Copyright (C) 2000-2003 Alistair Riddoch
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

#ifndef EQUATOR_RENDERER_H
#define EQUATOR_RENDERER_H

#include <sigc++/signal.h>

namespace WFMath {
  class Quaternion;
}

class Renderer {
  public:
    Renderer() { }

    void orient(const WFMath::Quaternion &);

    void fogOn() { }
    void fogOff() { }

    void lightOn() { }
    void lightOff() { }

    sigc::signal<void> Restart;
    sigc::signal<void, float> Update;
};

#endif // EQUATOR_RENDERER_H

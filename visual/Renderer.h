// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef EQUATOR_RENDERER_H
#define EQUATOR_RENDERER_H

#include <sigc++/signal.h>

class Renderer {
  public:
    Renderer() { }

    void fogOn() { }
    void fogOff() { }

    void lightOn() { }
    void lightOff() { }

    SigC::Signal1<void, float> Update;
};

#endif // EQUATOR_RENDERER_H

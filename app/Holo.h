// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_HOLO_H
#define EQUATOR_APP_HOLO_H

#include "Layer.h"

class Holo : public Layer {
  public:
    explicit Holo(const GlView &);
    void draw();
};

#endif // EQUATOR_APP_HOLO_H

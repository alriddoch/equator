// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "EntityLayer.h"
#include "GlView.h"

#include <GL/gl.h>

EntityLayer::EntityLayer(Model & model) : Layer(model, "background", "Entities")
{
}

void EntityLayer::draw(GlView & view)
{
}

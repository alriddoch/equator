// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Holo.h"
#include "GlView.h"

#include <GL/gl.h>

Holo::Holo(Model & model) : Layer(model, "background", "HoloWorld")
{
}

void Holo::draw(GlView & view)
{
    float winsize = std::max(view.width(), view.height());

    float winscale = view.getScale();
    int numlines = winsize / (40.0f * winscale) + 1;

    int incr = 1;
    if (winscale < 0.24f) {
        incr = 10;
    }

    glDepthMask(GL_FALSE);

    glBegin(GL_LINES);

    glColor3f(0.0f, 0.3f, 0.0f);

    for (int i = -(numlines - numlines % incr); i <= numlines; i += incr) {
        if (i == 0) continue;

        glVertex3f(-numlines, i, 0.0f);
        glVertex3f(numlines, i, 0.0f);

        glVertex3f(i, -numlines, 0.0f);
        glVertex3f(i, numlines, 0.0f);
    }

    glColor3f(0.0f, 1.0f, 0.0f);

    glVertex3f(-numlines, 0.0f, 0.0f);
    glVertex3f(numlines, 0.0f, 0.0f);

    glVertex3f(0.0f, -numlines, 0.0f);
    glVertex3f(0.0f, numlines, 0.0f);

    glEnd();

    glDepthMask(GL_TRUE);

}

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

    int numlines = winsize / (40.0f * view.getScale()) + 1;
    //glTranslatef(0.0f, 0.0f, -40.0f);
    //glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
    //glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-numlines, 0.0f, 0.0f);
    glVertex3f(numlines, 0.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0f, -numlines, 0.0f);
    glVertex3f(0.0f, numlines, 0.0f);
    glEnd();

    for (int i = -numlines; i <= numlines; i++) {
        if (i == 0) continue;
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.3f, 0.0f);
        glVertex3f(-numlines, i, 0.0f);
        glVertex3f(numlines, i, 0.0f);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(i, -numlines, 0.0f);
        glVertex3f(i, numlines, 0.0f);
        glEnd();
    }

}

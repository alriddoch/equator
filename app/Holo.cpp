// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Holo.h"
#include "GlView.h"

#include <GL/gl.h>

Holo::Holo(const GlView & window) : Layer(window, "background", "HoloWorld")
{
}

void Holo::draw()
{
    float winsize = std::max(m_window.width(), m_window.height());

    int numlines = winsize / (40.0f * m_window.getScale()) + 1;
    //glTranslatef(0.0f, 0.0f, -40.0f);
    //glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
    //glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
    for (int i = -numlines; i <= numlines; i++) {
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

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-numlines, 0.0f, 0.0f);
    glVertex3f(numlines, 0.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0f, -numlines, 0.0f);
    glVertex3f(0.0f, numlines, 0.0f);
    glEnd();

}

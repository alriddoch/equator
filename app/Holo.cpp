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
    float winsize = std::max(view.get_width(), view.get_height());

    float winscale = view.getScale();
    int numlines = (int)(winsize / (40.0f * winscale) + 1);
    int xc = -(int)view.getXoff();
    int yc = -(int)view.getYoff();

    int incr = 1;
    if (winscale < 0.24f) {
        incr = 10;
        xc = xc - xc % 10;
        yc = yc - yc % 10;
    }

    // This needs to be divided into two sections, one which generates the
    // vertex array, and the other which draws it, so we don't have to
    // regenerate the array every time the screen is refreshed.

    // THere is an imprecision here. We need a less unweildy calculation for the
    // number of lines we are going to draw

    // Actually this all just sucks - need to take a much more considered
    // approach to deciding what we draw.
    int numVertices = (numlines + 1) * 2 * 3 * 2 * 2;
    float origin_varray[] = { xc - numlines, 0.f, 0.f,
                              xc + numlines, 0.f, 0.f,
                              0.f, yc - numlines, 0.f,
                              0.f, yc + numlines, 0.f };
    float * varray = new float[numVertices];
    int vCount = -1;

    for (int i = -(numlines - numlines % incr); i <= numlines; i += incr) {
        varray[++vCount] = xc - numlines;
        varray[++vCount] = yc + i;
        varray[++vCount] = 0.0f;
        varray[++vCount] = xc + numlines;
        varray[++vCount] = yc + i;
        varray[++vCount] = 0.0f;

        varray[++vCount] = xc + i;
        varray[++vCount] = yc - numlines;
        varray[++vCount] = 0.0f;
        varray[++vCount] = xc + i;
        varray[++vCount] = yc + numlines;
        varray[++vCount] = 0.0f;
    }

    glEnable(GL_LINE_SMOOTH);

    // Draw the green grid.
    glColor3f(0.0f, 0.3f, 0.0f);
    glVertexPointer(3, GL_FLOAT, 0, varray);
    glDrawArrays(GL_LINES, 0, (vCount + 1) / 3);
    glDisable(GL_LINE_SMOOTH);

    // Draw the origin lines.
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertexPointer(3, GL_FLOAT, 0, origin_varray);
    glDrawArrays(GL_LINES, 0, 4);

    delete [] varray;

}

bool Holo::animate(GlView &)
{
    return false;
}

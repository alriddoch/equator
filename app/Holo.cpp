// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Holo.h"
#include "GlView.h"

#include "visual/StockTexture.h"
#include "visual/Texture.h"

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

    // THere is an imprecision here. We need a less unweildy calculation for the
    // number of lines we are going to draw

    int plane_varray[] = { xc - numlines, yc - numlines, 0,
                           xc - numlines, yc + numlines, 0,
                           xc + numlines, yc + numlines, 0,
                           xc + numlines, yc - numlines, 0 };

    static GLfloat sx0[] = {1.f, 0.f, 0.f, 0.f};
    static GLfloat ty0[] = {0.f, 1.f, 0.f, 0.f};


    GLuint holo_texture = getStockTexture(STOCK_HOLO);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, holo_texture);

    // Texture coord generation
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx0);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, ty0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);
    glColor3f(0.5f, 0.5f, 0.5f);

    glVertexPointer(3, GL_INT, 0, plane_varray);
    glDrawArrays(GL_QUADS, 0, 4);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

}

bool Holo::animate(GlView &)
{
    return false;
}

// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

#include "Holo.h"
#include "GlView.h"

#include "visual/StockTexture.h"
#include "visual/Texture.h"

Holo::Holo(Model & model) : Layer(model, "background", "HoloWorld"),
                            m_box_radius(8)
{
}

void Holo::setBoxSize(int r)
{
    m_box_radius = r;
}

void Holo::draw(GlView & view)
{
    float winsize = std::max(view.get_width(), view.get_height());

    float winscale = view.getScale();
    int numlines = (int)(winsize / (40.0f * winscale) + 1);
    int xc = -(int)view.getXoff();
    int yc = -(int)view.getYoff();

    if (m_box_radius > 0) {
        xc = 0;
        yc = 0;
        numlines = m_box_radius;
    }

    // THere is an imprecision here. We need a less unweildy calculation for the
    // number of lines we are going to draw

    int plane_varray[] = {
                           xc - numlines, yc - numlines, 0,
                           xc + numlines, yc - numlines, 0,
                           xc + numlines, yc + numlines, 0,
                           xc - numlines, yc + numlines, 0,

                           xc - numlines, yc + numlines, 0,
                           xc + numlines, yc + numlines, 0,
                           xc + numlines, yc + numlines, numlines,
                           xc - numlines, yc + numlines, numlines,

                           xc - numlines, yc - numlines, 0,
                           xc - numlines, yc - numlines, numlines,
                           xc + numlines, yc - numlines, numlines,
                           xc + numlines, yc - numlines, 0,

                           xc + numlines, yc - numlines, 0,
                           xc + numlines, yc - numlines, numlines,
                           xc + numlines, yc + numlines, numlines,
                           xc + numlines, yc + numlines, 0,

                           xc - numlines, yc - numlines, 0,
                           xc - numlines, yc + numlines, 0,
                           xc - numlines, yc + numlines, numlines,
                           xc - numlines, yc - numlines, numlines
                         };

    static GLfloat x0[] = {1.f, 0.f, 0.f, 0.f};
    static GLfloat y0[] = {0.f, 1.f, 0.f, 0.f};
    static GLfloat z0[] = {0.f, 0.f, 1.f, 0.f};


    GLuint holo_texture = getStockTexture(STOCK_HOLO);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, holo_texture);

    // Texture coord generation
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, x0);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, y0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glColor3f(0.5f, 0.5f, 0.5f);

    glVertexPointer(3, GL_INT, 0, plane_varray);
    glDrawArrays(GL_QUADS, 0, 4);

    if (m_box_radius > 0) {
        glTexGenfv(GL_S, GL_OBJECT_PLANE, x0);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, z0);
        glDrawArrays(GL_QUADS, 4, 8);
        glTexGenfv(GL_S, GL_OBJECT_PLANE, y0);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, z0);
        glDrawArrays(GL_QUADS, 12, 8);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

}

bool Holo::animate(GlView &)
{
    return false;
}

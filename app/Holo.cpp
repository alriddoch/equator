// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Holo.h"
#include "GlView.h"

#include "visual/StockTexture.h"
#include "visual/Texture.h"

Holo::Holo(Model & model) : Layer(model, "background", "HoloWorld"),
                            m_box_radius(0)
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
    int zc = -(int)view.getYoff();

    if (m_box_radius > 0) {
        numlines = m_box_radius;
    }

    // THere is an imprecision here. We need a less unweildy calculation for the
    // number of lines we are going to draw

    int plane_varray[] = {
                           xc - numlines, yc - numlines, 0,
                           xc + numlines, yc - numlines, 0,
                           xc + numlines, yc + numlines, 0,
                           xc - numlines, yc + numlines, 0,

                           xc - numlines, yc + numlines, zc - 0,
                           xc + numlines, yc + numlines, zc - 0,
                           xc + numlines, yc + numlines, zc + numlines,
                           xc - numlines, yc + numlines, zc + numlines,

                           xc - numlines, yc - numlines, zc - 0,
                           xc - numlines, yc - numlines, zc + numlines,
                           xc + numlines, yc - numlines, zc + numlines,
                           xc + numlines, yc - numlines, zc - 0,

                           xc + numlines, yc - numlines, zc - 0,
                           xc + numlines, yc - numlines, zc + numlines,
                           xc + numlines, yc + numlines, zc + numlines,
                           xc + numlines, yc + numlines, zc - 0,

                           xc - numlines, yc - numlines, zc - 0,
                           xc - numlines, yc + numlines, zc - 0,
                           xc - numlines, yc + numlines, zc + numlines,
                           xc - numlines, yc - numlines, zc + numlines
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

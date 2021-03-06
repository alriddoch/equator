// Equator Online RPG World Building Tool
// Copyright (C) 2004 Alistair Riddoch
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

#include "GameView.h"

#include "Model.h"
#include "WorldEntity.h"
#include "GlView.h"
#include "Server.h"

#include "visual/GL.h"
#include "visual/EntityRenderer.h"

#include "common/debug.h"

#include <Eris/View.h>

#include <iostream>
#include <cassert>

static const bool debug_flag = false;

GameView::GameView(Model & m, Server & s) : Layer(m, m.getName(), "Game View"),
                                            m_serverConnection(s)
{
}

void GameView::orient(const WFMath::Quaternion & orientation)
{
    if (!orientation.isValid()) {
        return;
    }
    float orient[4][4];
    WFMath::RotMatrix<3> omatrix(orientation); // .asMatrix(orient);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            orient[i][j] = omatrix.elem(i,j);
        }
    }
    orient[3][0] = orient[3][1] = orient[3][2] = orient[0][3] = orient[1][3] = orient[2][3] = 0.0f;
    orient[3][3] = 1.0f;
    glMultMatrixf(&orient[0][0]);
}

void GameView::drawEntity(GlView & view, Eris::Entity * ent,
                          RenderableEntity * pe, const PosType & cp)
// This code come from apogee. Effort should be made to track
// apogees Renderer::drawEntity()
{
    assert(ent != 0);

    float worldTime = 0; // FIXME Gratuitous hack

    PosType pos = ent->getPosition();
    WFMath::Quaternion orientation = ent->getOrientation();

    if (!orientation.isValid()) {
        orientation.identity();
    }

    MovableEntity * me = dynamic_cast<MovableEntity *>(ent);
    if (me != NULL) {
        debug( std::cout << ent->getVelocity() << " "
                         << (worldTime - me->getTime()) << " " << pos; );
        pos = pos + ent->getVelocity() * (double)((worldTime - me->getTime())/1000.0f);
        debug( std::cout << "=" << pos << std::endl << std::flush; );
        if (pe != 0) {
            pe->constrainChild(*me, pos);;
        }
    }

    PosType camPos = cp.toLocalCoords(pos, orientation);

    RenderableEntity * re = dynamic_cast<RenderableEntity *>(ent);
    if (re == 0) {
        return;
    }

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    orient(orientation);

    re->m_drawer->render(view.m_renderer, camPos);

    if (!re->m_drawer->drawContents()) {
        glPopMatrix();
        return;
    }

    int numEnts = ent->numContained();
    debug(std::cout << ent->getId() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getContained(i);
        if (!e->isVisible()) { continue; }
        // debug(std::cout << ":" << e->getId() << e->getPosition() << ":"
                        // << e->getBBox().u << e->getBBox().v
                        // << std::endl << std::flush;);
        drawEntity(view, e, re, camPos);
    }
    glPopMatrix();
}

void GameView::drawWorld(GlView & view, Eris::Entity * wrld)
{
    PosType camPos(-view.getXoff(), -view.getYoff(), -view.getZoff());

    drawEntity(view, wrld, 0, camPos);
}


// Standard Layer Interface Implementation
// The code below implements the virtual methods required by the Layer
// base class.

void GameView::options()
{
}

void GameView::importFile()
{
    // Nothing
}

void GameView::exportFile()
{
    // Nothing
}

void GameView::selectInvert()
{
    // Nothing
}

void GameView::selectAll()
{
    // Nothing
}

void GameView::selectNone()
{
    // Nothing
}

void GameView::draw(GlView & view)
{
    Eris::Entity * root = m_serverConnection.m_view->getTopLevel();
    drawWorld(view, root);
}

bool GameView::animate(GlView & view)
{
    // Nothing
    return false;
}

void GameView::select(GlView & view, int x, int y)
{
    // Nothing
}

void GameView::select(GlView & view, int x, int y, int fx, int fy)
{
    // Nothing
}

void GameView::pushSelection()
{
    // Nothing
}

void GameView::popSelection()
{
    // Nothing
}

void GameView::dragStart(GlView & view, int x, int y)
{
    // Nothing
}

void GameView::dragUpdate(GlView & view, const WFMath::Vector<3> & v)
{
    // Nothing
}

void GameView::dragEnd(GlView & view, const WFMath::Vector<3> & v)
{
    // Nothing
}

void GameView::insert(const PosType &)
{
}

void GameView::align(Alignment)
{
}

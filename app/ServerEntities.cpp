// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "ServerEntities.h"
#include "GlView.h"
#include "Server.h"
#include "Model.h"

#include "Vector3D.h"
#include "debug.h"

#include <Eris/Entity.h>
#include <Eris/World.h>

#include <GL/glu.h>

static const bool debug_flag = false;

void ServerEntities::draw3DCube(const Vector3D & coords,
                                const Eris::BBox & bbox, bool open)
{
    bool shaded = (m_renderMode == GlView::SHADED);
    glPushMatrix();

    glTranslatef(coords.X(), coords.Y(), coords.Z());

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    // Bottom face
    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);

    // Top face
    if (!open) {
        glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
        glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
        glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
        glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
    }

    if (!shaded) { glColor3f(0.2f, 0.2f, 1.0f); }
    // South face
    if (!open) {
        glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
        glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
        glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
        glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    }

    // North face
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);

    if (!shaded) { glColor3f(0.0f, 0.0f, 0.7f); }
    // West face
    if (!open) {
        glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
        glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
        glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
        glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    }

    // East face
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
    glEnd();

    glPopMatrix();
}

void ServerEntities::draw3DBox(const Vector3D & coords,
                               const Eris::BBox & bbox)
{
    glPushMatrix();
    // origin();
    glTranslatef(coords.X(), coords.Y(), coords.Z());

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);

    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);

    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);

    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);

    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);

    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);

    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);

    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);

    glVertex3f(bbox.u.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.u.z);

    glVertex3f(bbox.v.x,bbox.u.y,bbox.u.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.u.z);

    glVertex3f(bbox.u.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.u.x,bbox.v.y,bbox.v.z);

    glVertex3f(bbox.v.x,bbox.u.y,bbox.v.z);
    glVertex3f(bbox.v.x,bbox.v.y,bbox.v.z);
    glEnd();

    glPopMatrix();

}

void ServerEntities::drawEntity(Eris::Entity * ent)
{
    glPushMatrix();
    const Eris::Coord & pos = ent->getPosition();
    glTranslatef(pos.x, pos.y, pos.z);
    int numEnts = ent->getNumMembers();
    debug(std::cout << ent->getID() << " " << numEnts << " emts"
                    << std::endl << std::flush;);
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        debug(std::cout << ":" << e->getID() << e->getPosition() << ":"
                        << e->getBBox().u << e->getBBox().v
                        << std::endl << std::flush;);
        // if (!e->isVisible()) { continue; }
        switch (m_renderMode) {
            case GlView::LINE:
                draw3DBox(e->getPosition(), e->getBBox());
                break;
            case GlView::SOLID:
            case GlView::SHADED:
            case GlView::TEXTURE:
            case GlView::SHADETEXT:
            default:
                draw3DCube(e->getPosition(), e->getBBox());
        }
        //draw3DBox(e->getPosition(), e->getBBox());
        drawEntity(e);
    }
    glPopMatrix();
}

void ServerEntities::drawWorld(Eris::Entity * wrld)
{
    drawEntity(wrld);
}

void ServerEntities::selectEntity(Eris::Entity* ent,entlist_t::const_iterator I)
{
    glPushMatrix();
    const Eris::Coord & pos = ent->getPosition();
    glTranslatef(pos.x, pos.y, pos.z);
    int numEnts = ent->getNumMembers();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent->getMember(i);
        if (!e->isVisible()) { continue; }
        m_nameDict[++m_nameCount] = e;
        glPushName(m_nameCount);
        draw3DCube(e->getPosition(), e->getBBox());
        if (I != m_selectionStack.end()) {
            entlist_t::const_iterator J = I;
            selectEntity(e, ++J);
        }
        glPopName();
    }
}

void ServerEntities::selectSingleEntity(GlView & view,
                                        int nx, int ny, int fx, int fy,
                                        bool check)
{
    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);
    glRenderMode(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix(nx, ny, fx - nx, fy - ny, viewport);

    view.setPickProjection(); // Sets the projection, sets up names
                                  // and sets up the modelview

    m_nameCount = 0;
    m_nameDict.clear();

    entlist_t::const_iterator I = m_selectionStack.begin();

    Eris::Entity * root = m_serverConnection.world->getRootEntity();

    selectEntity(root, I);

    
}

ServerEntities::ServerEntities(Model & model, Server & server) :
                               Layer(model, model.getName(), "ServerEntities"),
                               m_serverConnection(server)
{
}

void ServerEntities::draw(GlView & view)
{
    float winsize = std::max(view.width(), view.height());

    m_renderMode = view.getRenderMode();
    Eris::Entity * root = m_serverConnection.world->getRootEntity();
    drawWorld(root);
}

void ServerEntities::select(GlView & view, int x, int y)
{
}

void ServerEntities::select(GlView & view, int x, int y, int w, int h)
{
}

void ServerEntities::pushSelection()
{
}

void ServerEntities::popSelection()
{
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IsoMap.h"
#include "Texture.h"
#include "GlView.h"
#include "Model.h"

#include <coal/database.h>
#include <coal/isoloader.h>
#include <coal/debug.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gtk--/fileselection.h>

#include <iostream>

void IsoMap::drawMapRegion(CoalRegion & map_region)
{
    Tile * tile = NULL;
    CoalFill * fill = map_region.GetFill();
    if (fill != NULL) {
        if ((fill->graphic != NULL) && (fill->graphic->filename.size() != 0)) {
            tile = Tile::get(fill->graphic->filename);
        }
        glNormal3f(0, 0, 1);
        CoalShape & shape = map_region;
        if (shape.GetCoordCount() != 2) {
            std::cout << "This don't look like a tile" << std::endl << std::flush;
        } else {
            //for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
            glPushMatrix();
            const CoalCoord & coord = shape.GetCoord(0);
            float bx = coord.GetX();
            float by = coord.GetY();
            float bz = coord.GetZ();
            glTranslatef(bx, by, bz);
            if (tile != NULL) {
                tile->draw();
            } else {
                CoalGraphic * fillGraphic = fill->graphic;
                float r = fillGraphic->bkgdcolor.red;
                float g = fillGraphic->bkgdcolor.green;
                float b = fillGraphic->bkgdcolor.blue;
                glBegin(GL_QUADS);                // start drawing a polygon
                glColor3f(r/100, g/100, b/100);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 1.0f, 0.0f);
                glVertex3f(0.0f, 1.0f, 0.0f);
                glEnd();
            }
            if (m_selection.find(&map_region) != m_selection.end()) {
                glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 0.5f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 0.0f, 0.0f);
                glVertex3f(1.0f, 1.0f, 0.0f);
                glVertex3f(1.0f, 1.0f, 0.0f);
                glVertex3f(0.0f, 1.0f, 0.0f);
                glVertex3f(0.0f, 1.0f, 0.0f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glEnd();
            }
            glPopMatrix();
        }
    }
}

void IsoMap::drawMapObject(CoalObject & map_object)
{
    
}

void IsoMap::drawMap(CoalDatabase & map_base)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    // glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    count = map_base.GetObjectCount();
    for (int i = 0; i < count; i++) {
        CoalObject * object = (CoalObject*)map_base.GetObject(i);
        if (object) {
            drawMapObject(*object);
        }
    }

}

bool IsoMap::selectMap(GlView & view, CoalDatabase & map_base,
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
    std::cout << "PICK " << nx << " " << ny << " " << fx - nx << " " << fy - ny << std::endl << std::flush;
    gluPickMatrix(nx, ny, fx - nx, fy - ny, viewport);

    view.setPickProjection(); // Sets the projection, sets up names
                                  // and sets up the modelview

    glTranslatef(m_xoff, m_yoff, m_zoff);

    int nameCount = 0;
    std::map<int, CoalRegion *> nameDict;
    int count = map_base.GetRegionCount();
    glPushName(nameCount);
    for (int i = 0; i < count; i++) {
        CoalRegion * shape = (CoalRegion*)map_base.GetRegion(i);

        if (shape == NULL) { continue; }
        CoalFill * fill = shape->GetFill();
        if ((fill == NULL) || (fill->graphic == NULL) ||
            (fill->graphic->filename.size() == 0)) {
            continue;
        }
        Tile * tile = Tile::get(fill->graphic->filename);
        if (tile == NULL) { continue; }
        nameDict[++nameCount] = shape;
        glLoadName(nameCount);
        // drawMapRegion(*region);
        const CoalCoord & coord = shape->GetCoord(0);
        float bx = coord.GetX();
        float by = coord.GetY();
        float bz = coord.GetZ();
        glPushMatrix();
        glTranslatef(bx, by, bz);
        tile->select();
        glPopMatrix();
    }
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    if (!check) {
        m_selection.clear();
    }

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return false; }

    if (check && m_selection.empty()) { return true; }

    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            std::cout << "{" << hitName << "}";
            std::map<int, CoalRegion *>::const_iterator I = nameDict.find(hitName);
            if (check) {
                if (m_selection.find(I->second) != m_selection.end()) {
                    return true;
                    std::cout << "SELECTION VERIFIED" << std::endl << std::flush;
                }
            } else {
                if (I != nameDict.end()) {
                    m_selection[I->second] = 0;
                } else {
                    std::cout << "UNKNOWN NAME" << std::endl << std::flush;
                }
            }
        }
    }
    std::cout << std::endl << std::flush;
    return !check;

}

void IsoMap::load(Gtk::FileSelection * fsel)
{
    CoalIsoLoader loader (m_database);

    std::string filename = fsel->get_filename();
    loader.LoadMap(filename.c_str());
    size_t i = filename.find_last_of('/');
    if (i == 0) {
        m_name = filename;
    } else {
        m_name = filename.substr(i+1);
    }
 

    delete fsel;

    m_model.updated.emit();
}

void IsoMap::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

IsoMap::IsoMap(Model & model) : Layer(model, "map", "IsoMap"),
                                        m_database(*new CoalDatabase()),
                                        m_validDrag(false)
{
}

void IsoMap::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Iso Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::load),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::cancel),fsel));
    fsel->show();
}

void IsoMap::draw(GlView &)
{
    glPushMatrix();
    glTranslatef(m_xoff, m_yoff, m_zoff);
    drawMap(m_database);
    glPopMatrix();
}

void IsoMap::select(GlView & view, int x, int y)
{
    select(view, x, y, x + 1, y + 1);
}

void IsoMap::select(GlView & view, int nx, int ny, int fx, int fy)
{
    selectMap(view, m_database, nx, ny, fx, fy);
}

void IsoMap::dragStart(GlView & view, int x, int y)
{
    if (selectMap(view, m_database, x, y, x+1, y+1, true)) {
        m_validDrag = true;
        if (m_selection.empty()) {
            std::cout << "Moving whole layer" << std::endl << std::flush;
            // Moving whole layer
        } else {
            std::cout << "Moving selection layer" << std::endl << std::flush;
            // Moving selection
        }
    } else {
            std::cout << "Moving nothing" << std::endl << std::flush;
        m_validDrag = false;
    }
}

void IsoMap::dragUpdate(GlView & ,float x, float y, float z)
{
}

void IsoMap::dragEnd(GlView &, float x, float y, float z)
{
    if (m_validDrag) {
        if (m_selection.empty()) {
            m_xoff += x;
            m_yoff += y;
            m_zoff += z;
        } else {
            // FIXME move the current selection
        }
        m_validDrag = false;
        m_model.updated.emit();
    }
}


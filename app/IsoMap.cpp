// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IsoMap.h"
#include "Texture.h"

#include <coal/database.h>
#include <coal/isoloader.h>
#include <coal/debug.h>

#include <GL/gl.h>

#include <gtk--/fileselection.h>

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
            cout << "This don't look like a tile" << endl << flush;
        } else {
            //for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
            glPushMatrix();
            //const CoalCoord & coord = shape.GetCoord(i);
            float bx = shape.GetCoord(0).GetX();
            float by = shape.GetCoord(0).GetY();
            float bz = shape.GetCoord(0).GetZ();
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
            glPopMatrix();
                //std::cout << "[" << x << "," << y << "z" << z << std::endl
                          //<< std::flush;
                //glTexCoord2f(0.5f, 1.0f); glVertex3f(bx, by, bz);
                //glTexCoord2f(1.0f, 0.5f); glVertex3f(tx, by, bz);
                //glTexCoord2f(0.5f, 0.0f); glVertex3f(tx, ty, bz);
                //glTexCoord2f(0.0f, 0.5f); glVertex3f(bx, ty, bz);
            //}
        }
    }
}

void IsoMap::drawMapObject(CoalObject & map_object)
{
    
}

void IsoMap::drawMap(CoalDatabase & map_base)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    glDepthMask(GL_TRUE);

    count = map_base.GetObjectCount();
    for (int i = 0; i < count; i++) {
        CoalObject * object = (CoalObject*)map_base.GetObject(i);
        if (object) {
            drawMapObject(*object);
        }
    }

}

void IsoMap::load(Gtk::FileSelection * fsel)
{
    CoalIsoLoader loader (m_database);
    loader.LoadMap(fsel->get_filename().c_str());
    CoalDebug debug;
    debug.Dump (m_database);

    delete fsel;
}

IsoMap::IsoMap(const GlView & window) : Layer(window, "map", "IsoMap"),
                                        m_database(*new CoalDatabase())
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::load),fsel));
    fsel->show();
}

void IsoMap::draw()
{
    drawMap(m_database);
}

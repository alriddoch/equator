// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "BladeMap.h"
#include "Texture.h"

#include <coal/database.h>
#include <coal/bladeloader.h>

#include <GL/gl.h>

#include <gtk--/fileselection.h>

void BladeMap::drawMapRegion(CoalRegion & map_region)
{
    int tex_id = -1;
    CoalFill * fill = map_region.GetFill();
    if (fill != NULL) {
        if ((fill->graphic != NULL) && (fill->graphic->filename.size() != 0)) {
            tex_id = Texture::get(fill->graphic->filename);
        }
        if (tex_id == -1) {
            CoalGraphic * fillGraphic = fill->graphic;
            float r = fillGraphic->bkgdcolor.red;
            float g = fillGraphic->bkgdcolor.green;
            float b = fillGraphic->bkgdcolor.blue;
            //std::cout << "[" << r << "," << g << "," << b
                      //<< std::endl << std::flush;
            glBegin(GL_POLYGON);                // start drawing a polygon
            glColor3f(r/100, g/100, b/100);
        } else {
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_POLYGON);                // start drawing a polygon
            glColor3f(1.0, 0.0, 1.0);
        }
    }
    glNormal3f(0, 0, 1);
    CoalShape & shape = map_region;
    for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
        const CoalCoord & coord = shape.GetCoord(i);
        float x = coord.GetX();
        float y = coord.GetY();
        float z = coord.GetZ();
        //std::cout << "[" << x << "," << y << "z" << z << std::endl
                  //<< std::flush;
        glTexCoord2f(x/16, y/16); glVertex3f(x, y, z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void BladeMap::drawMapObject(CoalObject & map_object)
{
    
}

void BladeMap::drawMap(CoalDatabase & map_base)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
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

void BladeMap::load(Gtk::FileSelection * fsel)
{
    CoalBladeLoader loader (m_database);
    loader.LoadMap(fsel->get_filename().c_str());

    delete fsel;
}

BladeMap::BladeMap(const GlView & window) : Layer(window, "map", "BladeMap"),
                                        m_database(*new CoalDatabase())
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &BladeMap::load),fsel));
    fsel->show();
}

void BladeMap::draw()
{
    drawMap(m_database);
}

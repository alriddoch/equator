// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "BladeMap.h"
#include "Texture.h"
#include "GlView.h"

#include <coal/database.h>
#include <coal/bladeloader.h>

#include <GL/glu.h>

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
    if (m_selection.find(&map_region) != m_selection.end()) {
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.5f);
        for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
            const CoalCoord & coord = shape.GetCoord(i);
            float x = coord.GetX();
            float y = coord.GetY();
            float z = coord.GetZ();
            //std::cout << "[" << x << "," << y << "z" << z << std::endl
                      //<< std::flush;
            glVertex3f(x, y, z);
        }
        glVertex3f(shape.GetCoord(0).GetX(), shape.GetCoord(1).GetY(), shape.GetCoord(2).GetZ());
        glEnd();
    }
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

void BladeMap::selectMap(CoalDatabase & map_base, int nx, int ny, int fx, int fy)
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

    m_window.setPickProjection(); // Sets the projection, sets up names
                                  // and sets up the modelview

    int nameCount = 0;
    std::map<int, CoalRegion *> nameDict;

    int count = map_base.GetRegionCount();
    glPushName(nameCount);
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region == NULL) { continue; }
        nameDict[++nameCount] = region;
        glLoadName(nameCount);
        glBegin(GL_POLYGON);                // start drawing a polygon
        CoalShape & shape = *region;
        for (unsigned int i = 0; i < shape.GetCoordCount (); i++) {
            const CoalCoord & coord = shape.GetCoord(i);
            float x = coord.GetX();
            float y = coord.GetY();
            float z = coord.GetZ();
            //std::cout << "[" << x << "," << y << "z" << z << std::endl
                      //<< std::flush;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    cout << "Got " << hits << " hits" << endl << flush;
    if (hits < 1) { return ; }
    m_selection.clear();

    // Need to find out which is the closest his under some circumstances,
    // as layers may overlap. This will be tricky. Probably need to
    // make it depend on whether this is an area selection or a click seletion.
    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            cout << "{" << hitName << "}";
            std::map<int, CoalRegion *>::const_iterator I = nameDict.find(hitName);
            if (I != nameDict.end()) {
                m_selection[I->second] = 0;
            } else {
                cout << "UNKNOWN NAME" << endl << flush;
            }
        }
    }
    cout << endl << flush;
    return;

}

void BladeMap::load(Gtk::FileSelection * fsel)
{
    CoalBladeLoader loader (m_database);
    loader.LoadMap(fsel->get_filename().c_str());

    delete fsel;
}

void BladeMap::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

BladeMap::BladeMap(GlView & window) : Layer(window, "map", "BladeMap"),
                                        m_database(*new CoalDatabase())
{
}

void BladeMap::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Blade Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &BladeMap::load),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &BladeMap::cancel),fsel));
    fsel->show();
}

void BladeMap::draw()
{
    drawMap(m_database);
}

void BladeMap::select(int x, int y)
{
    select(x, y, x + 1, y + 1);
}

void BladeMap::select(int nx, int ny, int fx, int fy)
{
    selectMap(m_database, nx, ny, fx, fy);
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "BladeMap.h"
#include "Texture.h"
#include "GlView.h"
#include "Model.h"
#include "MainWindow.h"
#include "Palette.h"

#include <coal/container.h>
#include <coal/bladeloader.h>

#include <GL/glu.h>

#include <gtkmm/fileselection.h>

#include <iostream>

using std::cout;

void BladeMap::drawMapRegion(Coal::Landscape & map_region)
{
    std::cout << "Drawing... " << map_region.getID() << std::endl << std::flush;
    Coal::Shape * cs = map_region.getShape();
    if (cs == NULL) {
        std::cout << "no shape" << std::endl << std::flush;
        return;
    }
    Coal::Graphic * cg = map_region.getGraphic();
    int tex_id = -1;
    if (cg == NULL) {
        std::cout << "no graphic" << std::endl << std::flush;
        return;
    }
    if (!cg->filename.empty()) {
        tex_id = Texture::get(cg->filename);
    }
    Coal::PolygonShape * cps = dynamic_cast<Coal::PolygonShape *>(cs);
    if (cps != NULL) {
        if (tex_id != -1) {
            glBegin(GL_POLYGON);
            glColor3f(cg->bkgdcolor.red / 255.0f,
                      cg->bkgdcolor.green / 255.0f,
                      cg->bkgdcolor.blue / 255.0f);
        } else {
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_POLYGON);                // start drawing a polygon
            glColor3f(1.0, 0.0, 1.0);
        }
        glNormal3f(0, 0, 1);
        // FIXME Not implemented yet
        glEnd();
        return;
    }
    Coal::RectShape * crs = dynamic_cast<Coal::RectShape *>(cs);
    if (crs != NULL) {
        if (tex_id == -1) {
            glColor3f(cg->bkgdcolor.red / 255.0f,
                      cg->bkgdcolor.green / 255.0f,
                      cg->bkgdcolor.blue / 255.0f);
        } else {
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glEnable(GL_TEXTURE_2D);
            glColor3f(1.0, 0.0, 1.0);
        }
        glNormal3f(0, 0, 1);

        const GLfloat texcoords[] = {
                            crs->getLeft() / 16.0f, crs->getTop() / 16.0f,
                            crs->getLeft() / 16.0f, crs->getBottom() / 16.0f,
                            crs->getRight() / 16.0f, crs->getBottom() / 16.0f,
                            crs->getRight() / 16.0f, crs->getTop() / 16.0f };
        const GLfloat vertices[] = {
                            crs->getLeft(), crs->getTop(), 0.0f,
                            crs->getLeft(), crs->getBottom(), 0.0f,
                            crs->getRight(), crs->getBottom(), 0.0f,
                            crs->getRight(), crs->getTop(), 0.0f };
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(3, GL_FLOAT, 0, texcoords);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        if (tex_id != -1) {
            glDisable(GL_TEXTURE_2D);
        }
        return;
    }
#if 0
    int tex_id = -1;
    Coal::Fill * fill = map_region.getFill();
    if (fill != NULL) {
        if ((fill->graphic != NULL) && (fill->graphic->filename.size() != 0)) {
            tex_id = Texture::get(fill->graphic->filename);
        }
        if (tex_id == -1) {
            Coal::Graphic * fillGraphic = fill->graphic;
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
    Coal::Shape & shape = map_region;
    for (unsigned int i = 0; i < shape.getCoordCount (); i++) {
        const Coal::Coord & coord = shape.getCoord(i);
        float x = coord.getX();
        float y = coord.getY();
        float z = coord.getZ();
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
            const Coal::Coord & coord = shape.GetCoord(i);
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
#endif
}

void BladeMap::drawMap(Coal::Container & map_base)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthMask(GL_FALSE);
    //glDisable(GL_DEPTH_TEST);
#if 0
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            drawMapRegion(*region);
        }
    }
    //glDepthMask(GL_TRUE);
    //glEnable(GL_DEPTH_TEST);
#else
    const std::vector<Coal::Component *> contents = map_base.getChildren();
    std::vector<Coal::Component *>::const_iterator I = contents.begin();
    for(; I != contents.end(); ++I) {
        // CoalComponent * c = I->Current();
        Coal::Landscape * l = dynamic_cast<Coal::Landscape *>(*I);
        if (l != NULL) {
            std::cout << "GOT ONE" << std::endl << std::flush;
            drawMapRegion(*l);
        } else {
            std::cout << "NOT DRAWING " << (*I)->getID() << std::endl << std::flush;
        }
    }
#endif
}

bool BladeMap::selectMap(GlView & view, Coal::Container & map_base,
                         int nx, int ny, int fx, int fy,
                         bool check)
{
    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

    int nameCount = 0;
    std::map<int, Coal::Component *> nameDict;

    glPushName(nameCount);

    Coal::Iterator * I = map_base.getIterator();
    for(I->first(); !I->isLast(); I->next()) {
#if 0
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
#endif
    }
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    if (!check) {
        m_selection.clear();
    }

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return false; }

    if (check && m_selection.empty()) { return true; }

    // Need to find out which is the closest his under some circumstances,
    // as layers may overlap. This will be tricky. Probably need to
    // make it depend on whether this is an area selection or a click seletion.
    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            std::cout << "{" << hitName << "}";
            std::map<int, Coal::Component *>::const_iterator I = nameDict.find(hitName);
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

void BladeMap::load(Gtk::FileSelection * fsel)
{
    Coal::BladeLoader loader;

    std::string filename = fsel->get_filename();
    loader.loadMap(filename.c_str(), &m_database);
    size_t i = filename.find_last_of('/');
    if (i == 0) {
        m_name = filename;
    } else {
        m_name = filename.substr(i+1);
    }

    installTextures();

    delete fsel;

    m_model.updated.emit();
}

void BladeMap::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void BladeMap::installTextures()
{
    // Palette & p = m_model.m_mainWindow.m_palettewindow;

#if 0
    int c = m_database.GetGraphicCount(); 
    for (int i = 0; i < c; i++) {
        CoalGraphic * g = m_database.GetGraphic(i);
        cout << "GRAPHIC: " << g->name << ":" << g->filename
             << std::endl << std::flush;
        p.addTextureEntry(&m_model, g->name);
#warning TODO Add code to coal to add short names for tiles
    }
#endif
    m_model.typesAdded.emit();
    // p.addTileEntry();
}

BladeMap::BladeMap(Model & model) : Layer(model, "map", "BladeMap"),
                                        m_database(*new Coal::Container()),
                                        m_validDrag(false)
{
}

void BladeMap::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Blade Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &BladeMap::load),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &BladeMap::cancel),fsel));
    fsel->show();
}

void BladeMap::draw(GlView &)
{
    glPushMatrix();
    glTranslatef(m_xoff, m_yoff, m_zoff);
    drawMap(m_database);
    glPopMatrix();
}

void BladeMap::select(GlView & view, int x, int y)
{
    select(view, x, y, x + 1, y + 1);
}

void BladeMap::select(GlView & view, int nx, int ny, int fx, int fy)
{
    selectMap(view, m_database, nx, ny, fx, fy);
}

void BladeMap::dragStart(GlView & view, int x, int y)
{
    if (selectMap(view, m_database, x, y, x+1, y+1, true)) {
        m_validDrag = true;
        if (m_selection.empty()) {
            // Moving whole layer
        } else {
            // Moving selection
        }
    } else {
        m_validDrag = false;
    }
}

void BladeMap::dragUpdate(GlView & view, float x, float y, float z)
{
}

void BladeMap::dragEnd(GlView & view, float x, float y, float z)
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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "IsoMap.h"
#include "Texture.h"
#include "GlView.h"
#include "Model.h"
#include "MainWindow.h"
#include "Palette.h"
#include "TileMap.h"

#include <coal/database.h>
#include <coal/isoloader.h>
#include <coal/debug.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <gtk--/fileselection.h>

#include <fstream>

class Palette;

void IsoMap::buildTileMap(CoalDatabase & map_base)
{
    m_tileMap = new TileMap();
    int count = map_base.GetRegionCount();
    for (int i = 0; i < count; i++) {
        CoalRegion * region = (CoalRegion*)map_base.GetRegion(i);
        if (region) {
            CoalFill * fill = region->GetFill();
            if ((fill == NULL) || (fill->graphic == NULL) ||
                (fill->graphic->filename.size() == 0)) {
                continue;
            }
            Tile * tile = Tile::get(fill->graphic->filename);
            if (tile == NULL) {
                continue;
            }
            CoalShape & shape = *region;
            const CoalCoord & coord = shape.GetCoord(0);
            float bx = coord.GetX();
            float by = coord.GetY();
            m_tileMap->add((int)bx, (int)by, tile);
        }
    }
}

void IsoMap::drawMap(GlView & view)
// This version uses the TileMap
{
    // Insert tile code at this point
    int size = (int)view.getViewSize() / 2;

    for(int i = -size; i < size; i++) {
        for(int j = -size; j < size; j++) {
            int x = (int) - view.getXoff() + i;
            int y = (int) - view.getXoff() + j;
            Tile * t = m_tileMap->get(x, y);
            if (t == NULL) { continue; }
            glPushMatrix();
            glTranslatef(x, y, 0);
            // t->draw(map_height, x, y);
            t->draw(m_model.m_heightData, x, y);
            glPopMatrix();
        }
    }
}

void IsoMap::animateMap(GlView & view, float count)
{
    // Insert tile code at this point
    int size = (int)view.getViewSize() / 2;

    for(int i = -size; i < size; i++) {
        for(int j = -size; j < size; j++) {
            int x = (int) - view.getXoff() + i;
            int y = (int) - view.getXoff() + j;
            Tile * t = m_tileMap->get(x, y);
            if (t == NULL) { continue; }
            glPushMatrix();
            glTranslatef(x, y, 0);
            if (m_selection.find(GroundCoord(x,y)) != m_selection.end()) {
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_TEXTURE_1D);
                glBindTexture(GL_TEXTURE_1D, m_antTexture);
                t->outline(count);
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_TEXTURE_1D);
            }
            glPopMatrix();
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
    int sWidth = abs(fx - nx);
    int sHeight = abs(fy - ny);
    int sXCentre = (fx > nx) ? (nx + sWidth / 2) : (fx + sWidth / 2);
    int sYCentre = (fy > ny) ? (ny + sHeight / 2) : (fy + sHeight / 2);
    gluPickMatrix(sXCentre, sYCentre, sWidth, sHeight, viewport);

    view.setPickProjection(); // Sets the projection, sets up names
                                  // and sets up the modelview

    glTranslatef(m_xoff, m_yoff, m_zoff);

    int nameCount = 0;
    std::map<int, GroundCoord> nameDict;
    glPushName(nameCount);

    int size = (int)view.getViewSize() / 2;

    for(int i = -size; i < size; i++) {
        for(int j = -size; j < size; j++) {
            int x = (int) - view.getXoff() + i;
            int y = (int) - view.getXoff() + j;
            Tile * t = m_tileMap->get(x, y);
            if (t == NULL) { continue; }
            nameDict[++nameCount] = GroundCoord(x,y);
            glLoadName(nameCount);
            glPushMatrix();
            glTranslatef(x, y, 0);
            // t->draw(map_height, x, y);
            t->draw();
            glPopMatrix();
        }
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
            std::map<int, GroundCoord>::const_iterator I = nameDict.find(hitName);
            if (check) {
                if (m_selection.find(I->second) != m_selection.end()) {
                    return true;
                    std::cout << "SELECTION VERIFIED" << std::endl << std::flush;
                }
            } else {
                if (I != nameDict.end()) {
                    m_selection.insert(I->second);
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
 
    installTiles();
    buildTileMap(m_database);

    delete fsel;

    m_model.updated.emit();
}

void IsoMap::save(Gtk::FileSelection * fsel)
{
    std::string filename = fsel->get_filename();

    delete fsel;

    std::ofstream exportFile(filename.c_str());

    if (!exportFile.is_open()) {
        std::cerr << "ERROR: Could not open file " << filename
                  << " for export" << std::endl << std::flush;
        return;
    }
    exportFile << "Map: 200 200 70 36 map" << std::endl;
    
    std::set<Tile *> tileSet;
    const int mapwidth = 200;

    for(int i = mapwidth - 1; i >= 0; --i) {
        for(int j = mapwidth - 1; j >= 0; --j) {
            Tile * t = m_tileMap->get(i, j);
            if (t == NULL) { continue; }
            if (tileSet.find(t) != tileSet.end()) { continue; }
            tileSet.insert(t);
        }
    }

    exportFile << "TileList: " << tileSet.size() << std::endl;

    std::map<Tile *, int> usedTiles;
    int tileNo = -1;

    std::set<Tile *>::const_iterator I = tileSet.begin();
    for(; I != tileSet.end(); I++) {
            usedTiles[*I] = ++tileNo;
            exportFile << "TileGfx: " << tileNo << " 0 0 " << (*I)->m_name
                       << std::endl;
    }

    for(int i = mapwidth - 1; i >= 0; --i) {
        for(int j = mapwidth - 1; j >= 0; --j) {
            Tile * t = m_tileMap->get(i, j);
            std::map<Tile*, int>::const_iterator J = usedTiles.find(t);
            if (J == usedTiles.end()) {
                std::cerr << "ERROR: Tile map changed unexpectedly"
                          << std::endl << std::flush;
                continue;
            }
            tileNo = J->second;
            exportFile << "Tile: 1 " << tileNo << " 0 0 0" << std::endl;
        }
    }
}

void IsoMap::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void IsoMap::installTiles()
{
    Palette & p = m_model.m_mainWindow.m_palettewindow;

    int c = m_database.GetGraphicCount(); 
    for (int i = 0; i < c; i++) {
        CoalGraphic * g = m_database.GetGraphic(i);
        p.addTileEntry(&m_model, g->filename);
#warning TODO Add code to coal to add short names for tiles
    }
    m_model.typesAdded.emit();
    // p.addTileEntry();
}

IsoMap::IsoMap(Model & model) : Layer(model, "map", "IsoMap"),
                                        m_database(*new CoalDatabase()),
                                        m_validDrag(false),
                                        m_tileMap(NULL)
{
}

void IsoMap::importFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Iso Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::load),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::cancel),fsel));
    fsel->show();
}

void IsoMap::exportFile()
{
    Gtk::FileSelection * fsel = new Gtk::FileSelection("Save Iso Map File");
    fsel->get_ok_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::save),fsel));
    fsel->get_cancel_button()->clicked.connect(SigC::bind<Gtk::FileSelection*>(slot(this, &IsoMap::cancel),fsel));
    fsel->show();
}

void IsoMap::draw(GlView & view)
{
    if (m_tileMap == NULL) {
        return;
    }
    m_antTexture = view.getAnts();
    glPushMatrix();
    glTranslatef(m_xoff, m_yoff, m_zoff);
    drawMap(view);
    glPopMatrix();
}

void IsoMap::animate(GlView & view)
{
    if (m_tileMap == NULL) {
        return;
    }
    m_antTexture = view.getAnts();
    glPushMatrix();
    glTranslatef(m_xoff, m_yoff, m_zoff);
    animateMap(view, view.getAnimCount());
    glPopMatrix();
}

void IsoMap::select(GlView & view, int x, int y)
{
    if (m_tileMap == NULL) {
        return;
    }
    select(view, x, y, x + 1, y + 1);
}

void IsoMap::select(GlView & view, int nx, int ny, int fx, int fy)
{
    if (m_tileMap == NULL) {
        return;
    }
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


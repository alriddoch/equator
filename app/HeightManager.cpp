// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "HeightManager.h"

#include "Model.h"
#include "GlView.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include <gtkmm/fileselection.h>

#include <GL/glu.h>

#include <iostream>

void HeightManager::load(Gtk::FileSelection * fsel)
{
    float x, y, z;
    m_model.getCursor(x, y, z);
    int posx = x / segSize, posy = y / segSize;
    if (x < 0) { posx -= 1; }
    if (y < 0) { posy -= 1; }
    // FIXME Do we even wanna load anymore?
    m_model.m_terrain.setBasePoint(posx,     posy,     -10.0);
    m_model.m_terrain.setBasePoint(posx + 1, posy,     16.0);
    m_model.m_terrain.setBasePoint(posx,     posy + 1, 20.0);
    m_model.m_terrain.setBasePoint(posx + 1, posy + 1, 40.0);
    m_model.m_terrain.refresh(posx,     posy);
    m_model.m_terrain.refresh(posx + 1, posy);
    m_model.m_terrain.refresh(posx,     posy + 1);
    m_model.m_terrain.refresh(posx + 1, posy + 1);
    // m_model.m_heightData.load(fsel->get_filename(),posx,posy);
    // FIXME Handle error conditions from height loader

    delete fsel;

    m_model.updated.emit();
}

void HeightManager::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

HeightManager::HeightManager(Model &m) : Layer(m, "heightfield", "HeightField"),
                       m_numLineIndeces(0),
                       m_lineIndeces(new unsigned int[segSize * segSize * 2])
{
    int idx = -1;
    for (int i = 0; i < segSize - 1; ++i) {
        for (int j = 0; j < segSize; ++j) {
            m_lineIndeces[++idx] = j * segSize + i;
            m_lineIndeces[++idx] = j * segSize + i + 1;
        }
        if (++i >= segSize - 1) { break; }
        for (int j = segSize - 1; j >= 0; --j) {
            m_lineIndeces[++idx] = j * segSize + i;
            m_lineIndeces[++idx] = j * segSize + i + 1;
        }
    }
    m_numLineIndeces = ++idx;
}

void HeightManager::importFile()
{

    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Height Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &HeightManager::load),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &HeightManager::cancel),fsel));
    fsel->show();

}

void HeightManager::selectRegion(Mercator::Segment * map)
{
    int size = map->getSize();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(size / 2.0f, size / 2.0f ,0.0f);
    for (int i = 0; i < size; ++i) {
        glVertex3f(i, 0.0f, map->get(i, 0));
    }
    for (int i = 0; i < size; ++i) {
        glVertex3f(size - 1, i, map->get(size - 1, i));
    }
    for (int i = size - 1; i >= 0; --i) {
        glVertex3f(i, size - 1, map->get(i, size - 1));
    }
    for (int i = size - 1; i >= 0; --i) {
        glVertex3f(0.0f, i, map->get(0, i));
    }
    glEnd();
}

void HeightManager::outlineLineStrip(float * varray, unsigned int size,
                                     float count)
{
    float * tarray = new float[size];
    for(int i = 0; i < size; ++i) {
        tarray[i] = count + i;
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, varray);
    glTexCoordPointer(1, GL_FLOAT, 0, tarray);
    glDrawArrays(GL_LINE_STRIP, 0, size);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void HeightManager::heightMapRegion(Mercator::Segment * map)
{
    float * harray = new float[segSize * segSize * 3];
    float * carray = new float[segSize * segSize * 3];
    const float * points = map->getPoints();
    int idx = -1, cdx = -1;
    for(int j = 0; j < segSize; ++j) {
        for(int i = 0; i < segSize; ++i) {
            float h = map->get(i,j);
            harray[++idx] = i;
            harray[++idx] = j;
            harray[++idx] = h;
            if (h > 0) {
                carray[++cdx] = -0.5f + h / 20.0f;
                carray[++cdx] = 0.5f + h / 20.0f;
                carray[++cdx] = -1.5f + h / 20.0f;
            } else {
                carray[++cdx] = 0.f;
                carray[++cdx] = 0.8 + h / 20.0f;
                carray[++cdx] = 1.0f + h / 2000.0f;
            }
        }
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, harray);
    glColorPointer(3, GL_FLOAT, 0, carray);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, segSize * segSize);
    }
    glDrawElements(GL_LINE_STRIP, m_numLineIndeces,
                   GL_UNSIGNED_INT, m_lineIndeces);
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void HeightManager::drawRegion(GlView & view, Mercator::Segment * map)
{
    float * varray = new float[segSize * 4 * 3];
    int vdx = -1;
    for (int i = 0; i < segSize; ++i) {
        varray[++vdx] = i;
        varray[++vdx] = 0.0f;
        varray[++vdx] = map->get(i, 0);
    }
    for (int i = 0; i < segSize; ++i) {
        varray[++vdx] = segSize - 1;
        varray[++vdx] = i;
        varray[++vdx] = map->get(segSize - 1, i);
    }
    for (int i = segSize - 1; i >= 0; --i) {
        varray[++vdx] = i;
        varray[++vdx] = segSize - 1;
        varray[++vdx] = map->get(i, segSize - 1);
    }
    for (int i = segSize - 1; i >= 0; --i) {
        varray[++vdx] = 0.0f;
        varray[++vdx] = i;
        varray[++vdx] = map->get(0, i);
    }
    bool selected = (m_selection.find(map) != m_selection.end());
    if (selected) {
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, view.getAnts());
        outlineLineStrip(varray, segSize * 4, view.getAnimCount());
        glDisable(GL_TEXTURE_1D);
        // if (view.getScale() > 0.05f) {
            heightMapRegion(map);
        // }
        return;
    }
    glColor3f(1.0f, 0.0f, 0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, varray);
    glDrawArrays(GL_LINE_STRIP, 0, segSize * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void HeightManager::draw(GlView & view)
{
    const Mercator::Terrain::Segmentstore & segs = m_model.m_terrain.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            drawRegion(view, J->second);
            glPopMatrix();
        }
    }
}

void HeightManager::animate(GlView & view)
#if 0
{
    HeightData::HeightMapGrid::const_iterator I = m_model.m_heightData.begin();
    for (; I != m_model.m_heightData.end(); ++I) {
        if (m_selection.find(I->second) == m_selection.end()) {
            continue;
        }
        glPushMatrix();
        const GroundCoord & coord = I->first;
        glTranslatef(coord.first * 200.0f, coord.second * 200.0f, 0.0f);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, view.getAnts());
        outlineRegion(I->second, view.getAnimCount());
        glDisable(GL_TEXTURE_1D);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix();
    }
}
#else
{
    const Mercator::Terrain::Segmentstore & segs = m_model.m_terrain.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            std::cout << "Animating segment at " << I->first << "," << J->first
                      << std::endl << std::flush;
            // FIXME Actually do some animation maybe?
            drawRegion(view, J->second);
            glPopMatrix();
        }
    }
}
#endif

void HeightManager::select(GlView & view, int x, int y)
{
    select(view, x, y, x + 1, y + 1);
}

void HeightManager::select(GlView & view, int nx, int ny, int fx, int fy)
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

    int nameCount = 0;
    std::map<int, GroundCoord> nameDict;
    glPushName(nameCount);

    const Mercator::Terrain::Segmentstore & segs = m_model.m_terrain.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            nameDict[++nameCount] = GroundCoord(I->first, J->first);
            glLoadName(nameCount);
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            selectRegion(J->second);
            glPopMatrix();
        }
    }
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    m_selection.clear();

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return; }

    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            std::cout << "{" << hitName << "}";
            std::map<int, GroundCoord>::const_iterator K = nameDict.find(hitName);
            if (K != nameDict.end()) {
                const GroundCoord & c = K->second;
                m_selection.insert(m_model.m_terrain.getSegmentSafe(c.first,
                                                                    c.second));
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
    std::cout << std::endl << std::flush;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "HeightManager.h"

#include "Model.h"
#include "MainWindow.h"
#include "GlView.h"

#include "arrow_mesh.h"

#include <wfmath/point.h>

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include <gtkmm/fileselection.h>

#include <GL/glu.h>

#include <iostream>
#include <math.h>

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
                       m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2])
{
    int idx = -1;
    for (int i = 0; i < segSize; ++i) {
        for (int j = 0; j < segSize + 1; ++j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
        }
        if (++i >= segSize) { break; }
        for (int j = segSize; j >= 0; --j) {
            m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
            m_lineIndeces[++idx] = j * (segSize + 1) + i;
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
    float vertices[size * 4 * 3 + 1];
    vertices[0] = vertices[1] = size / 2.f;
    vertices[2] = 0.f;

    for (int i = 0; i < size; ++i) {
        vertices[3 * i + 1] = i;
        vertices[3 * i + 2] = 0.f;
        vertices[3 * i + 3] = map->get(i, 0);

        vertices[3 * (size + i) + 1] = size - 1;
        vertices[3 * (size + i) + 2] = i;
        vertices[3 * (size + i) + 3] = map->get(size - 1, i);

        vertices[3 * (2 * size + i) + 1] = size - 1 - i;
        vertices[3 * (2 * size + i) + 2] = size - 1;
        vertices[3 * (2 * size + i) + 3] = map->get(size - 1 - i, size - 1);

        vertices[3 * (3 * size + i) + 1] = 0.f;
        vertices[3 * (3 * size + i) + 2] = size - 1 - i;
        vertices[3 * (3 * size + i) + 3] = map->get(0.f, size - 1 -i);
    }
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, size * 4 + 1);
}

void HeightManager::outlineLineStrip(float * varray, unsigned int size,
                                     float count)
{
    float * tarray = new float[size];
    for(int i = 0; i < size; ++i) {
        tarray[i] = count + i;
    }
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, varray);
    glTexCoordPointer(1, GL_FLOAT, 0, tarray);
    glDrawArrays(GL_LINE_STRIP, 0, size);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    delete tarray;
}

void HeightManager::heightMapRegion(GlView & view, Mercator::Segment * map)
{
    float * const harray = new float[(segSize + 1) * (segSize + 1) * 3];
    float * const carray = new float[(segSize + 1) * (segSize + 1) * 3];
    const float * points = map->getPoints();
    int idx = -1, cdx = -1;
    for(int j = 0; j < segSize + 1; ++j) {
        for(int i = 0; i < segSize + 1; ++i) {
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
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, harray);
    glColorPointer(3, GL_FLOAT, 0, carray);
    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, (segSize + 1) * (segSize + 1));
    }
    if (view.getRenderMode(m_name) == GlView::LINE) {
        glDrawElements(GL_LINE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
    } else {
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
    }
    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }
    glDisableClientState(GL_COLOR_ARRAY);
    delete harray;
    delete carray;
}

void HeightManager::drawRegion(GlView & view, Mercator::Segment * map)
{
    float * varray = new float[(segSize + 1) * 4 * 3];
    int vdx = -1;
    for (int i = 0; i < (segSize + 1); ++i) {
        varray[++vdx] = i;
        varray[++vdx] = 0.0f;
        varray[++vdx] = map->get(i, 0);
    }
    for (int i = 0; i < (segSize + 1); ++i) {
        varray[++vdx] = (segSize + 1) - 1;
        varray[++vdx] = i;
        varray[++vdx] = map->get((segSize + 1) - 1, i);
    }
    for (int i = (segSize + 1) - 1; i >= 0; --i) {
        varray[++vdx] = i;
        varray[++vdx] = (segSize + 1) - 1;
        varray[++vdx] = map->get(i, (segSize + 1) - 1);
    }
    for (int i = (segSize + 1) - 1; i >= 0; --i) {
        varray[++vdx] = 0.0f;
        varray[++vdx] = i;
        varray[++vdx] = map->get(0, i);
    }
    bool selected = (m_selection.find(map) != m_selection.end());
    if (selected) {
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, view.getAnts());
        outlineLineStrip(varray, (segSize + 1) * 4, view.getAnimCount());
        glDisable(GL_TEXTURE_1D);
        // if (view.getScale() > 0.05f) {
            heightMapRegion(view, map);
        // }
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertexPointer(3, GL_FLOAT, 0, varray);
        glDrawArrays(GL_LINE_STRIP, 0, (segSize + 1) * 4);
    }
    delete varray;
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
            Mercator::Segment * s = J->second;
            if (!s->isValid()) {
                s->populate();
            }
            drawRegion(view, s);
            glPopMatrix();
        }
    }

    glEnable(GL_BLEND);
    I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            if (m_selection.find(J->second) == m_selection.end()) {
                continue;
            }
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            GLfloat vertices[] = { 0.f, 0.f, 0.f,
                                   segSize, 0, 0.f,
                                   segSize, segSize, 0.f,
                                   0, segSize, 0.f };
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glColor4f(1.f, 1.f, 1.f, 0.3f);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);

    if ((m_model.getCurrentLayer() != this) ||
        (m_model.m_mainWindow.getMode() != MainWindow::VERTEX)) {
        return;
    }

    glColor3f(1.0f, 0.f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);

    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, arrow_mesh_size);
    }

    float scale = 0.00625 / view.getScale();
    
    const Mercator::Terrain::Pointstore & points = m_model.m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator K = points.begin();
    for(; K != points.end(); ++K) {
        const Mercator::Terrain::Pointcolumn & col = K->second;
        Mercator::Terrain::Pointcolumn::const_iterator L = col.begin();
        for (; L != col.end(); ++L) {
            glPushMatrix();
            glTranslatef(K->first * segSize,
                         L->first * segSize,
                         L->second.height());
            glScalef(scale, scale, scale);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, arrow_mesh_size);
            glPopMatrix();
        }
    }

    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
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

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

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
                m_selection.insert(m_model.m_terrain.getSegment(c.first,
                                                                c.second));
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
    std::cout << std::endl << std::flush;
}

typedef std::map<int, GroundCoord > BasepointSelection;

void HeightManager::dragStart(GlView & view, int x, int y)
{
    if (m_model.m_mainWindow.getMode() != MainWindow::VERTEX) {
        return;
    }

    GLuint selectBuf[32768];
    glSelectBuffer(32768, selectBuf);

    view.setPickProjection(x, y, x + 1, y + 1);

    int nameCount = 0;
    BasepointSelection nameDict;
    glPushName(nameCount);

    glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);

    if (have_GL_EXT_compiled_vertex_array) {
        glLockArraysEXT(0, arrow_mesh_size);
    }

    float scale = 0.00625 / view.getScale();
    
    const Mercator::Terrain::Pointstore & points = m_model.m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator K = points.begin();
    for(; K != points.end(); ++K) {
        const Mercator::Terrain::Pointcolumn & col = K->second;
        Mercator::Terrain::Pointcolumn::const_iterator L = col.begin();
        for (; L != col.end(); ++L) {
            nameDict[++nameCount] = GroundCoord(K->first, L->first);
            glLoadName(nameCount);
            glPushMatrix();
            glTranslatef(K->first * segSize,
                         L->first * segSize,
                         L->second.height());
            glScalef(scale, scale, scale);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, arrow_mesh_size);
            glPopMatrix();
        }
    }

    if (have_GL_EXT_compiled_vertex_array) {
        glUnlockArraysEXT();
    }

    glPopName();

    int hits = glRenderMode(GL_RENDER);

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return; }

    GLuint * ptr = &selectBuf[0];
    for (int i = 0; i < hits; i++) {
        int names = *(ptr);
        ptr += 3;
        for (int j = 0; j < names; j++) {
            int hitName = *(ptr++);
            std::cout << "{" << hitName << "}";
            std::map<int, GroundCoord>::const_iterator K = nameDict.find(hitName
);
            if (K != nameDict.end()) {
                const GroundCoord & c = K->second;
                std::cout << "DRAGGING " << c.first << "," << c.second
                          << std::endl << std::flush;
                m_dragPoint = c;
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }

}

void HeightManager::dragUpdate(GlView & view, float x, float y, float z)
{
}

void HeightManager::dragEnd(GlView & view, float x, float y, float z)
{
    Mercator::BasePoint ref;
    if (!m_model.m_terrain.getBasePoint(m_dragPoint.first,
                                        m_dragPoint.second, ref)) {
        return;
    }
    std::cout << "DRAGGED " << m_dragPoint.first << "," << m_dragPoint.second
              << " FROM " << ref.height()
              << " TO " << ref.height() + z
              << std::endl << std::flush;
    ref.height() += z; 
    m_model.m_terrain.setBasePoint(m_dragPoint.first, m_dragPoint.second, ref);
}

void HeightManager::insert(const WFMath::Point<3> & curs)
{
    int posx = ::round(curs.x() / segSize);
    int posy = ::round(curs.y() / segSize);
    std::cout << "Setting height at " << posx << "," << posy
              << " to " << curs.z()
              << std::endl << std::flush;
    m_model.m_terrain.setBasePoint(posx,     posy,     curs.z());
}

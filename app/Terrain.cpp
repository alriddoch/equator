// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "GL.h"

#include "Terrain.h"

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

void Terrain::load(Gtk::FileSelection * fsel)
{
    float x, y, z;
    m_model.getCursor(x, y, z);
    int posx = (int)(x / segSize), posy = (int)(y / segSize);
    if (x < 0) { posx -= 1; }
    if (y < 0) { posy -= 1; }
    // FIXME Do we even wanna load anymore?
    m_terrain.setBasePoint(posx,     posy,     -10.0);
    m_terrain.setBasePoint(posx + 1, posy,     16.0);
    m_terrain.setBasePoint(posx,     posy + 1, 20.0);
    m_terrain.setBasePoint(posx + 1, posy + 1, 40.0);
    // m_model.m_heightData.load(fsel->get_filename(),posx,posy);
    // FIXME Handle error conditions from height loader

    delete fsel;

    m_model.updated.emit();
}

void Terrain::cancel(Gtk::FileSelection * fsel)
{
    delete fsel;
}

void Terrain::initIndeces()
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

Terrain::Terrain(Model & m, Mercator::Terrain & mt) :
     Layer(m, "terrain", "Terrain"), m_terrain(mt), m_numLineIndeces(0),
     m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]), m_validDrag(false)
{
    initIndeces();
}

Terrain::Terrain(Model &m) : Layer(m, "terrain", "Terrain"),
     m_terrain(*new Mercator::Terrain), m_numLineIndeces(0),
     m_lineIndeces(new unsigned int[(segSize + 1) * (segSize + 1) * 2]), m_validDrag(false)
{
    initIndeces();
}

void Terrain::options()
{
}

void Terrain::importFile()
{

    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Height Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &Terrain::load),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &Terrain::cancel),fsel));
    fsel->show();

}

void Terrain::exportFile()
{
}

void Terrain::selectInvert()
{
}

void Terrain::selectAll()
{
}

void Terrain::selectNone()
{
    m_selection.clear();
}

void Terrain::selectRegion(Mercator::Segment & map)
{
    int size = map.getResolution();
    float vertices[size * 4 * 3 + 1];
    vertices[0] = vertices[1] = size / 2.f;
    vertices[2] = 0.f;

    for (int i = 0; i < size; ++i) {
        vertices[3 * i + 1] = i;
        vertices[3 * i + 2] = 0.f;
        vertices[3 * i + 3] = map.get(i, 0);

        vertices[3 * (size + i) + 1] = size - 1;
        vertices[3 * (size + i) + 2] = i;
        vertices[3 * (size + i) + 3] = map.get(size - 1, i);

        vertices[3 * (2 * size + i) + 1] = size - 1 - i;
        vertices[3 * (2 * size + i) + 2] = size - 1;
        vertices[3 * (2 * size + i) + 3] = map.get(size - 1 - i, size - 1);

        vertices[3 * (3 * size + i) + 1] = 0.f;
        vertices[3 * (3 * size + i) + 2] = size - 1 - i;
        vertices[3 * (3 * size + i) + 3] = map.get(0, size - 1 -i);
    }
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, size * 4 + 1);
}

void Terrain::outlineLineStrip(float * varray, unsigned int size,
                                     float count)
{
    float * tarray = new float[size];
    for(unsigned int i = 0; i < size; ++i) {
        tarray[i] = count + i;
    }
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, varray);
    glTexCoordPointer(1, GL_FLOAT, 0, tarray);
    glDrawArrays(GL_LINE_STRIP, 0, size);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    delete tarray;
}

void Terrain::heightMapRegion(GlView & view, Mercator::Segment & map)
{
    float * const harray = new float[(segSize + 1) * (segSize + 1) * 3];
    float * const carray = new float[(segSize + 1) * (segSize + 1) * 3];
    int idx = -1, cdx = -1;
    for(int j = 0; j < segSize + 1; ++j) {
        for(int i = 0; i < segSize + 1; ++i) {
            float h = map.get(i,j);
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
    if (view.getRenderMode(m_name) == GlView::LINE) {
        glDrawElements(GL_LINE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
    } else {
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_INT, m_lineIndeces);
    }
    glDisableClientState(GL_COLOR_ARRAY);
    delete harray;
    delete carray;
}

void Terrain::drawRegion(GlView & view, Mercator::Segment & map,
                         const GroundCoord & gc)
{
    float * varray = new float[(segSize + 1) * 4 * 3];
    int vdx = -1;
    for (int i = 0; i < (segSize + 1); ++i) {
        varray[++vdx] = i;
        varray[++vdx] = 0.0f;
        varray[++vdx] = map.get(i, 0);
    }
    for (int i = 0; i < (segSize + 1); ++i) {
        varray[++vdx] = (segSize + 1) - 1;
        varray[++vdx] = i;
        varray[++vdx] = map.get((segSize + 1) - 1, i);
    }
    for (int i = (segSize + 1) - 1; i >= 0; --i) {
        varray[++vdx] = i;
        varray[++vdx] = (segSize + 1) - 1;
        varray[++vdx] = map.get(i, (segSize + 1) - 1);
    }
    for (int i = (segSize + 1) - 1; i >= 0; --i) {
        varray[++vdx] = 0.0f;
        varray[++vdx] = i;
        varray[++vdx] = map.get(0, i);
    }
    bool selected = (m_selection.find(gc) != m_selection.end());
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

void Terrain::draw(GlView & view)
{
    const Mercator::Terrain::Segmentstore & segs = m_terrain.getTerrain();

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
            drawRegion(view, *s, GroundCoord(I->first, J->first));
            glPopMatrix();
        }
    }

    if ((m_model.getCurrentLayer() != this) ||
        (m_model.m_mainWindow.getMode() != MainWindow::VERTEX)) {
        return;
    }

    glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);

    float scale = 0.00625 / view.getScale();
    
    const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator K = points.begin();
    for(; K != points.end(); ++K) {
        const Mercator::Terrain::Pointcolumn & col = K->second;
        Mercator::Terrain::Pointcolumn::const_iterator L = col.begin();
        for (; L != col.end(); ++L) {
            GroundCoord gc(K->first, L->first);
            if ((m_selection.find(GroundCoord(K->first, L->first)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first - 1, L->first)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first - 1, L->first - 1)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first, L->first - 1)) == m_selection.end())) {
                continue;
            }
            if (m_vertexSelection.find(gc) != m_vertexSelection.end()) {
                glColor3f(1.f, 1.f, 0.f);
            } else {
                glColor3f(1.f, 0.f, 1.f);
            }
            glPushMatrix();
            glTranslatef(K->first * segSize,
                         L->first * segSize,
                         L->second.height());
            glScalef(scale, scale, scale);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, arrow_mesh_size);
            glPopMatrix();
        }
    }
}

bool Terrain::animate(GlView & view)
{
    if (m_selection.empty()) {
        return false;
    }
    if ((m_model.getCurrentLayer() == this) &&
        (m_model.m_mainWindow.getMode() == MainWindow::VERTEX)) {
        if (m_vertexSelection.empty()) {
            return false;
        }
        glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);
        glDepthFunc(GL_LEQUAL);
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        float phase = view.getAnimCount();
        glTranslatef(phase, phase, phase);
        glMatrixMode(GL_MODELVIEW);
        
        GLfloat s_scale = 0.5f * view.getScale();
        GLfloat sx[] = { s_scale, s_scale, s_scale, 0 };

        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, view.getAnts());
        glColor3f(1.f, 1.f, 1.f);
        glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv(GL_S, GL_OBJECT_PLANE, sx);

        float scale = 0.00625 / view.getScale();
    
        const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
        Mercator::Terrain::Pointstore::const_iterator K = points.begin();
        for(; K != points.end(); ++K) {
            const Mercator::Terrain::Pointcolumn & col = K->second;
            Mercator::Terrain::Pointcolumn::const_iterator L = col.begin();
            for (; L != col.end(); ++L) {
                GroundCoord gc(K->first, L->first);
                if ((m_selection.find(GroundCoord(K->first, L->first)) == m_selection.end()) &&
                    (m_selection.find(GroundCoord(K->first - 1, L->first)) == m_selection.end()) &&
                    (m_selection.find(GroundCoord(K->first - 1, L->first - 1)) == m_selection.end()) &&
                    (m_selection.find(GroundCoord(K->first, L->first - 1)) == m_selection.end())) {
                    continue;
                }
                if (m_vertexSelection.find(gc) == m_vertexSelection.end()) {
                    continue;
                }
                glPushMatrix();
                glTranslatef(K->first * segSize,
                             L->first * segSize,
                             L->second.height());
                if (m_validDrag) {
                    glTranslatef(0.f, 0.f, m_dragPoint.z());
                }
                glScalef(scale, scale, scale);
                glDrawArrays(GL_LINE_STRIP, 0, arrow_mesh_size);
                glPopMatrix();
            }
        }

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_1D);

        glDepthFunc(GL_LESS);

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        return true;
    } else {
        const Mercator::Terrain::Segmentstore & segs = m_terrain.getTerrain();
        Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
        for (; I != segs.end(); ++I) {
            const Mercator::Terrain::Segmentcolumn & col = I->second;
            Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
            for (; J != col.end(); ++J) {
                glPushMatrix();
                glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
                if (m_validDrag) {
                    glTranslatef(0.f, 0.f, m_dragPoint.z());
                }
                drawRegion(view, *J->second, GroundCoord(I->first, J->first));
                glPopMatrix();
            }
        }
    }
    return false;
}

bool Terrain::selectSegments(GlView & view, int nx, int ny, int fx, int fy, bool check)
{

    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

    int nameCount = 0;
    std::map<int, GroundCoord> nameDict;
    glPushName(nameCount);

    const Mercator::Terrain::Segmentstore & segs = m_terrain.getTerrain();

    Mercator::Terrain::Segmentstore::const_iterator I = segs.begin();
    for (; I != segs.end(); ++I) {
        const Mercator::Terrain::Segmentcolumn & col = I->second;
        Mercator::Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            nameDict[++nameCount] = GroundCoord(I->first, J->first);
            glLoadName(nameCount);
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            selectRegion(*J->second);
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
                if (check) {
                    return true;
                }
                m_selection.insert(c);
                view.startAnimation();
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
    std::cout << std::endl << std::flush;
    return false;
}

bool Terrain::selectBasepoints(GlView & view, int nx, int ny, int fx, int fy, bool check)
{

    GLuint selectBuf[32768];

    glSelectBuffer(32768,selectBuf);

    // Sets the projection, sets up names and sets up the modelview
    view.setPickProjection(nx, ny, fx, fy);

    int nameCount = 0;
    std::map<int, GroundCoord> nameDict;
    glPushName(nameCount);

    glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);

    float scale = 0.00625 / view.getScale();
    
    const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator K = points.begin();
    for(; K != points.end(); ++K) {
        const Mercator::Terrain::Pointcolumn & col = K->second;
        Mercator::Terrain::Pointcolumn::const_iterator L = col.begin();
        for (; L != col.end(); ++L) {
            if ((m_selection.find(GroundCoord(K->first, L->first)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first - 1, L->first)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first - 1, L->first - 1)) == m_selection.end()) &&
                (m_selection.find(GroundCoord(K->first, L->first - 1)) == m_selection.end())) {
                continue;
            }
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
    glPopName();

    int hits = glRenderMode(GL_RENDER);

    if (!check) {
        m_vertexSelection.clear();
    }

    std::cout << "Got " << hits << " hits" << std::endl << std::flush;
    if (hits < 1) { return false; }

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
                if (check) {
                    return true;
                }
                m_vertexSelection.insert(c);
                view.startAnimation();
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
    std::cout << std::endl << std::flush;
    return false;
}

void Terrain::select(GlView & view, int x, int y)
{
    select(view, x, y, x + 1, y + 1);
}

void Terrain::select(GlView & view, int nx, int ny, int fx, int fy)
{
    if (m_model.m_mainWindow.getMode() == MainWindow::VERTEX) {
        selectBasepoints(view, nx, ny, fx, fy);
    } else {
        selectSegments(view, nx, ny, fx, fy);
    }
}

typedef std::map<int, GroundCoord > BasepointSelection;

void Terrain::dragStart(GlView & view, int x, int y)
{
    if (m_model.m_mainWindow.getMode() == MainWindow::VERTEX) {
        if (selectBasepoints(view, x, y, x + 1, y + 1, true)) {
            m_validDrag = true;
            m_dragPoint = WFMath::Vector<3>(0.f, 0.f, 0.f);
        }
    } else {
        if (selectSegments(view, x, y, x + 1, y + 1, true)) {
            m_validDrag = true;
            m_dragPoint = WFMath::Vector<3>(0.f, 0.f, 0.f);
        }
    }

#if 0
    m_vertexSelection.clear();

    GLuint selectBuf[32768];
    glSelectBuffer(32768, selectBuf);

    view.setPickProjection(x, y, x + 1, y + 1);

    int nameCount = 0;
    BasepointSelection nameDict;
    glPushName(nameCount);

    glVertexPointer(3, GL_FLOAT, 0, arrow_mesh);

    float scale = 0.00625 / view.getScale();
    
    const Mercator::Terrain::Pointstore & points = m_terrain.getPoints();
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
                m_vertexSelection.insert(c);
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
#endif
}

void Terrain::dragUpdate(GlView & view, const WFMath::Vector<3> & v)
{
    m_dragPoint = v;
}

void Terrain::alterBasepoint(const GroundCoord & pt, float diff)
{
    Mercator::BasePoint ref;
    if (!m_terrain.getBasePoint(pt.first, pt.second, ref)) {
        return;
    }
    std::cout << "DRAGGED " << pt.first
              << "," << pt.second
              << " FROM " << ref.height()
              << " TO " << ref.height() + diff
              << std::endl << std::flush;
    ref.height() += diff; 
    m_terrain.setBasePoint(pt.first, pt.second, ref);
}

void Terrain::dragEnd(GlView & view, const WFMath::Vector<3> & v)
{
    if (!m_validDrag) {
        return;
    }
    std::set<GroundCoord> & selection = 
        (m_model.m_mainWindow.getMode() == MainWindow::VERTEX)
        ?  m_vertexSelection : m_selection;
    GroundCoordSet::const_iterator I = selection.begin();
    for(; I != selection.end(); ++I) {
        const GroundCoord & coord = *I;
#if 0
        Mercator::BasePoint ref;
        if (!m_terrain.getBasePoint(coord.first,
                                            coord.second, ref)) {
            continue;
        }
        std::cout << "DRAGGED " << coord.first
                  << "," << coord.second
                  << " FROM " << ref.height()
                  << " TO " << ref.height() + v.z()
                  << std::endl << std::flush;
        ref.height() += v.z(); 
        m_terrain.setBasePoint(coord.first, coord.second, ref);
#endif
        alterBasepoint(coord, v.z());
        if (m_model.m_mainWindow.getMode() == MainWindow::VERTEX) {
            continue;
        }
        // FIXME Cause things to get moved more than once.
        alterBasepoint(GroundCoord(coord.first + 1, coord.second), v.z());
        alterBasepoint(GroundCoord(coord.first, coord.second + 1), v.z());
        alterBasepoint(GroundCoord(coord.first + 1, coord.second + 1), v.z());

    }
    m_validDrag = false;
}

void Terrain::insert(const PosType & curs)
{
    int posx = (int)::round(curs.x() / segSize);
    int posy = (int)::round(curs.y() / segSize);
    std::cout << "Setting height at " << posx << "," << posy
              << " to " << curs.z()
              << std::endl << std::flush;
    m_terrain.setBasePoint(posx,     posy,     curs.z());
}

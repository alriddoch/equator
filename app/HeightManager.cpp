// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "HeightManager.h"

#include "Model.h"
#include "HeightData.h"
#include "HeightMap.h"
#include "GlView.h"

#include <gtkmm/fileselection.h>

#include <GL/glu.h>

#include <iostream>

void HeightManager::load(Gtk::FileSelection * fsel)
{
    float x, y, z;
    m_model.getCursor(x, y, z);
    int posx = x / HeightData::m_gridSize, posy = y / HeightData::m_gridSize;
    if (x < 0) { posx -= 1; }
    if (y < 0) { posy -= 1; }
    m_model.m_heightData.load(fsel->get_filename(),posx,posy);
    // FIXME Handle error conditions from height loader

    delete fsel;

    m_model.updated.emit();
}

void HeightManager::cancel(Gtk::FileSelection * fsel)
{
}

HeightManager::HeightManager(Model &m) :
                              Layer(m, "heightfield", "HeightField")
{
}

void HeightManager::importFile()
{

    Gtk::FileSelection * fsel = new Gtk::FileSelection("Load Height Map File");
    fsel->get_ok_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &HeightManager::load),fsel));
    fsel->get_cancel_button()->signal_clicked().connect(SigC::bind<Gtk::FileSelection*>(slot(*this, &HeightManager::cancel),fsel));
    fsel->show();

}

void HeightManager::selectRegion(HeightMap * map)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(100.0f,100.0f,0.0f);
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glVertex3f(i, 0.0f, map->get(i, 0) / 32.0f);
    }
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glVertex3f(199.0f, i, map->get(199, i) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glVertex3f(i, 199.0f, map->get(i, 199) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glVertex3f(0.0f, i, map->get(0, i) / 32.0f);
    }
    glEnd();
}

void HeightManager::outlineRegion(HeightMap * map, float count)
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glTexCoord1f(i + count);
        glVertex3f(i, 0.0f, map->get(i, 0) / 32.0f);
    }
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glTexCoord1f(i + count);
        glVertex3f(199.0f, i, map->get(199, i) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glTexCoord1f(i + count);
        glVertex3f(i, 199.0f, map->get(i, 199) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glTexCoord1f(i + count);
        glVertex3f(0.0f, i, map->get(0, i) / 32.0f);
    }
    glEnd();
}

void HeightManager::heightMapRegion(HeightMap * map)
{
    glBegin(GL_LINE_STRIP);
    glColor3f(0.5f, 0.0f, 0.0f);
    for (int i = 0; i < HeightData::m_gridSize - 1; ++i) {
        for (int j = 0; j < HeightData::m_gridSize; ++j) {
            glVertex3f(i, j, map->get(i, j) / 32.0f);
            glVertex3f(i + 1, j, map->get(i + 1, j) / 32.0f);
        }
        if (++i >= HeightData::m_gridSize - 1) { break; }
        for (int j = HeightData::m_gridSize - 1; j >= 0; --j) {
            glVertex3f(i, j, map->get(i, j) / 32.0f);
            glVertex3f(i + 1, j, map->get(i + 1, j) / 32.0f);
        }
    }
    glEnd();
}

void HeightManager::drawRegion(GlView & view, HeightMap * map)
{
    bool selected = (m_selection.find(map) != m_selection.end());
    if (selected) {
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, view.getAnts());
        outlineRegion(map, view.getAnimCount());
        glDisable(GL_TEXTURE_1D);
        if (view.getScale() > 0.24f) {
            heightMapRegion(map);
        }
        return;
    }
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glVertex3f(i, 0.0f, map->get(i, 0) / 32.0f);
    }
    for (int i = 0; i < HeightData::m_gridSize; ++i) {
        glVertex3f(199.0f, i, map->get(199, i) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glVertex3f(i, 199.0f, map->get(i, 199) / 32.0f);
    }
    for (int i = HeightData::m_gridSize - 1; i >= 0; --i) {
        glVertex3f(0.0f, i, map->get(0, i) / 32.0f);
    }
    glEnd();
}

void HeightManager::draw(GlView & view)
{
    HeightData::HeightMapGrid::const_iterator I = m_model.m_heightData.begin();
    for (; I != m_model.m_heightData.end(); ++I) {
        glPushMatrix();
        const GroundCoord & coord = I->first;
        glTranslatef(coord.first * 200.0f, coord.second * 200.0f, 0.0f);
        drawRegion(view, I->second);
        glPopMatrix();
    }
}

void HeightManager::animate(GlView & view)
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

    HeightData::HeightMapGrid::const_iterator I = m_model.m_heightData.begin();
    for (; I != m_model.m_heightData.end(); ++I) {
        const GroundCoord & coord = I->first;
        nameDict[++nameCount] = coord;
        glLoadName(nameCount);
        glPushMatrix();
        glTranslatef(coord.first * 200.0f, coord.second * 200.0f, 0.0f);
        selectRegion(I->second);
        glPopMatrix();
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
            std::map<int, GroundCoord>::const_iterator I = nameDict.find(hitName);
            if (I != nameDict.end()) {
                HeightData::HeightMapGrid::const_iterator J = m_model.m_heightData.find(I->second);
                if (J != m_model.m_heightData.end()) {
                    m_selection.insert(J->second);
                }
            } else {
                std::cout << "UNKNOWN NAME" << std::endl << std::flush;
            }
        }
    }
    std::cout << std::endl << std::flush;
}

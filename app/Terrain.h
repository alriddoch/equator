// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef EQUATOR_TERRAIN_H
#define EQUATOR_TERRAIN_H

#include "Layer.h"

#include <wfmath/vector.h>

#include <sigc++/signal.h>

#include <set>

namespace Gtk {
  class FileSelection;
}

namespace Mercator {
  class Terrain;
  class Segment;
}

namespace Eris {
  class Entity;
}

typedef std::set<GroundCoord> GroundCoordSet;

class Terrain : public Layer {
  public:
    Mercator::Terrain & m_terrain;
  private:

    GroundCoordSet m_selection;
    GroundCoordSet m_vertexSelection;

    int m_numLineIndeces;
    unsigned int * const m_lineIndeces;

    bool m_validDrag;
    WFMath::Vector<3> m_dragPoint;

    void initIndeces();

    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);

    void selectRegion(Mercator::Segment & map);
    void heightMapRegion(GlView & view, Mercator::Segment & map);
    void populateVertices(float *, Mercator::Segment &);
    void animateRegion(GlView & view, Mercator::Segment & map);
    void drawRegion(GlView & view, Mercator::Segment & map,
                    const GroundCoord & gc);
    void alterBasepoint(const GroundCoord &, float);

    bool selectSegments(GlView &, int, int, int, int, bool check = false);
    bool selectBasepoints(GlView &, int, int, int, int, bool check = false);
  public:
    static const int segSize = 64;
    explicit Terrain(Model &, Mercator::Terrain &);
    explicit Terrain(Model &);

    void readTerrain(const Eris::Entity & ent);

    virtual void options();
    virtual void importFile();
    virtual void exportFile();
    virtual void selectInvert();
    virtual void selectAll();
    virtual void selectNone();

    virtual void draw(GlView & view);
    virtual bool animate(GlView & view);
    virtual void select(GlView & view, int x, int y);
    virtual void select(GlView & view, int x, int y, int w, int h);
    virtual void pushSelection() { }
    virtual void popSelection() { }
    virtual void dragStart(GlView & view, int x, int y);
    virtual void dragUpdate(GlView & view, const WFMath::Vector<3> &);
    virtual void dragEnd(GlView & view, const WFMath::Vector<3> &);
    virtual void insert(const PosType &);
    virtual void align(Alignment ) { }

    sigc::signal<void> TerrainModified;
};

#endif // EQUATOR_TERRAIN_H

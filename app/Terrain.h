// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_TERRAIN_H
#define EQUATOR_TERRAIN_H

#include "Layer.h"

#include <set>

namespace Gtk {
  class FileSelection;
}

namespace Mercator {
  class Terrain;
  class Segment;
}

class Terrain : public Layer {
  public:
    Mercator::Terrain & m_terrain;
  private:

    std::set<GroundCoord> m_selection;
    GroundCoord m_dragPoint;

    int m_numLineIndeces;
    unsigned int * const m_lineIndeces;

    void initIndeces();

    void load(Gtk::FileSelection *);
    void cancel(Gtk::FileSelection *);

    void selectRegion(Mercator::Segment & map);
    void outlineLineStrip(float *, unsigned int, float);
    void heightMapRegion(GlView & view, Mercator::Segment & map);
    void drawRegion(GlView & view, Mercator::Segment & map,
                    const GroundCoord & gc);
  public:
    static const int segSize = 64;
    explicit Terrain(Model &, Mercator::Terrain &);
    explicit Terrain(Model &);

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
};

#endif // EQUATOR_TERRAIN_H

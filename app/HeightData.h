// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_HEIGHTDATA_H
#define EQUATOR_HEIGHTDATA_H

struct SDL_Surface;

#include "types.h"

#include <map>
#include <string>
#include <stdint.h>

class HeightMap;

class HeightData {
  public:
    typedef std::map<GroundCoord, HeightMap *> HeightMapGrid;
    static const int m_gridSize = 200;
  private:
    std::map<GroundCoord, HeightMap *> m_heightMaps;

  public:
    HeightData();

    HeightMapGrid::const_iterator begin() const {
        return m_heightMaps.begin();
    }

    HeightMapGrid::const_iterator find(const GroundCoord & gc) const {
        return m_heightMaps.find(gc);
    }

    HeightMapGrid::const_iterator end() const {
        return m_heightMaps.end();
    }

    bool load(const std::string & file, int, int);
    bool insert(HeightMap *, int, int);
    bool remove(int, int);
    bool move(int, int, int, int);
    HeightMapGrid::const_iterator find(int x, int y) const;
    int get(int x, int y) const;

};

#endif // EQUATOR_HEIGHTDATA_H

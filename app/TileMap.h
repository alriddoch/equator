// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef EQUATOR_TILEMAP_H
#define EQUATOR_TILEMAP_H

class Tile;

#include <map>

#include "types.h"

class TileMap {
  private:
    std::map<GroundCoord, Tile *> m_tiles;
  public:
    TileMap();

    void add(int x, int y, Tile * tile) {
        m_tiles[GroundCoord(x,y)] = tile;
    }

    Tile * get(int x, int y) {
        std::map<GroundCoord, Tile *>::const_iterator I =
                                              m_tiles.find(GroundCoord(x,y));
        if (I != m_tiles.end()) { return I->second; }
        return NULL;
    }
};

#endif // EQUATOR_TILEMAP_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "HeightData.h"

#include "HeightMap.h"

HeightData::HeightData()
{
}

bool HeightData::load(const std::string & file, int x, int y)
{
    HeightMap * hm = new HeightMap();
    if (hm->load(file)) {
        m_heightMaps[GroundCoord(x,y)] = hm;
    }
    return false;
}

HeightData::HeightMapGrid::const_iterator HeightData::find(int x, int y) const
{
    int posx = x / m_gridSize, posy = y / m_gridSize;
    if (x < 0) { posx -= 1; }
    if (y < 0) { posy -= 1; }
    return m_heightMaps.find(GroundCoord(posx, posy));
}

int HeightData::get(int x, int y) const
{
    HeightData::HeightMapGrid::const_iterator I = find(x,y);
    if (I == m_heightMaps.end()) { return 0; }
    int fx = x % m_gridSize, fy = y % m_gridSize;
    if (fx < 0) { fx += m_gridSize; }
    if (fy < 0) { fy += m_gridSize; }
    return I->second->get(fx,fy);
}


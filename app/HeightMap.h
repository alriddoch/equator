// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_HEIGHTMAP_H
#define EQUATOR_HEIGHTMAP_H

#include <string>
#include <stdint.h>

class HeightMap {
  private:
    uint8_t ** m_rowPointers;
    uint32_t m_width;
    uint32_t m_height;

  public:
    HeightMap();

    bool load(const std::string & file);
    int get(int x, int y) const;
};

#endif // EQUATOR_HEIGHTMAP_H

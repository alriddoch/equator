// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef EQUATOR_TILE_H
#define EQUATOR_TILE_H

struct SDL_Surface;

namespace Mercator {
    class Terrain;
}

#include <string>
#include <map>

class Tile {
  private:
    static unsigned int twoN(unsigned int);

    static std::map<std::string, Tile *> & tiles() {
        if (tiledb == NULL) {
            tiledb = new std::map<std::string, Tile *>;
        }
        return *tiledb;
    }

    static std::map<std::string, Tile *> * tiledb;

    int tex_id;
    int tileSize;
    float m_pw, m_ph;
  public:
    std::string m_name;

    static Tile * get(const std::string & filename);

    Tile() : tex_id(-1), m_pw(0), m_ph(0) { }

    bool load(const std::string & filename);
    void draw();
    void draw(const Mercator::Terrain &, int, int);
    void select();
    void outline(float);

    bool loaded() const { return (tex_id != -1); }
    float pw() { return m_pw; }
    float ph() { return m_ph; }
};

#endif // EQUATOR_TILE_H

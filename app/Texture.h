// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef APOGEE_VISUAL_TEXTURE_H
#define APOGEE_VISUAL_TEXTURE_H

struct SDL_Surface;

#include <string>
#include <map>

class Texture {
  private:
    Texture() { }

    static std::map<std::string, int> & textures() {
        if (texturedb == NULL) {
            texturedb = new std::map<std::string, int>;
        }
        return *texturedb;
    }
    static struct SDL_Surface * imageLoad(const std::string & filename);

    static std::map<std::string, int> * texturedb;
  public:
    static int loadTexture(struct SDL_Surface * image);
    static struct SDL_Surface * imageTransform(struct SDL_Surface * image);
    static int get(const std::string & filename);
};

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
    float tileSize;
    float m_pw, m_ph;
  public:
    static Tile * get(const std::string & filename);

    Tile() : tex_id(-1), m_pw(0), m_ph(0) { }

    bool load(const std::string & filename);
    void draw();

    bool loaded() const { return (tex_id != -1); }
    float pw() { return m_pw; }
    float ph() { return m_ph; }
};

#endif // APOGEE_VISUAL_TEXTURE_H

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

#endif // APOGEE_VISUAL_TEXTURE_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#define USE_SDL

#include "HeightMap.h"

#include <SDL_image.h>

#include <iostream>

HeightMap::HeightMap() : m_heightImage(NULL), m_buffer(NULL), m_pitch(0)
{
}

bool HeightMap::load(const std::string & file)
{
#ifdef USE_SDL
    m_heightImage = IMG_Load(file.c_str());
    if (m_heightImage == NULL) {
        std::cerr << "Failed to load heightmap " << file << " "
                  << SDL_GetError() << std::endl << std::flush;
        return false;
    }

    int bpp = m_heightImage->format->BitsPerPixel;

    if (bpp != 8) {
        std::cerr << "Height map is " << bpp << " not greyscale"
                  << std::endl << std::flush;
        return false;
    }
    return true;
#else
    FILE *img_fp = fopen(file, "rb");
    if (!img_fp) {
        std::cerr << "Image file " << file << " could not be opened."
                  << std::endl << std::flush;
        perror("fopen");
        return false;
    }

    char header[8];
    fread(header, 1, 8, img_fp);
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        std::cerr << "Image file " << file << " does not seem to be a png."
                  << std::endl << std::flush;
        return false;
    }
#endif
}

int HeightMap::get(int x, int y) const
{
    assert(m_heightImage != NULL);

    Uint8 * pixelbuf = (Uint8 *)m_heightImage->pixels;
    Uint16 pitch = m_heightImage->pitch;
    int width = m_heightImage->w;
    int height = m_heightImage->h;

    int ix = x;
    int iy = height - y;

    return *(pixelbuf + (iy * pitch + ix)) - 127;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "StockTexture.h"

#include "Texture.h"
#include "GLU.h"

#include <map>
#include <iostream>

#include "holo_texture.h"

typedef std::map<StockTexture, GLuint> StockTextureMap;

static StockTextureMap * stockTextures = 0;

GLuint getStockTexture(StockTexture st)
{
    if (stockTextures == 0) {
        stockTextures = new StockTextureMap;
    }

    StockTextureMap::const_iterator I = stockTextures->find(st);
    if (I != stockTextures->end()) {
        return I->second;
    }

    std::cout << "Creating new stock texture" << std::endl << std::flush;
    GLuint newTexture;
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);

    switch (st) {
      case STOCK_HOLO:
        gluBuild2DMipmaps(GL_TEXTURE_2D, texture_holo_internalFormat,
                          texture_holo_width, texture_holo_height,
                          texture_holo_format, GL_UNSIGNED_BYTE,
                          texture_holo_pixels);
        std::cout << "Creating holo stock texture" << std::endl << std::flush;
        break;
      default:
        glDeleteTextures(1, &newTexture);
        newTexture = Texture::getDefault();
        (*stockTextures)[st] = newTexture;
        return newTexture;
        break;
    }
    if (glGetError() != 0) {
        std::cerr << "ERROR: Failed to load stock texture" << std::endl << std::flush;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    (*stockTextures)[st] = newTexture;
    return newTexture;

}

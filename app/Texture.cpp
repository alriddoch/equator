// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Texture.h"

#include <SDL_image.h>
#include <GL/gl.h>

std::map<std::string, int> * Texture::texturedb = NULL;

SDL_Surface * Texture::imageLoad(const std::string & filename)
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
{
    SDL_Surface *image;

    image = IMG_Load(filename.c_str());
    if ( image == NULL ) {
        std::cerr << "Unable to load" << filename.c_str() << ":"
                  << SDL_GetError() << std::endl << std::flush;
        return(NULL);
    }
    return imageTransform(image);
}

SDL_Surface * Texture::imageTransform(SDL_Surface * image)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf;
    int i;

    /* GL surfaces are upsidedown and RGB, not BGR :-) */
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
#if 0
        for ( j=0; j<image->w; ++j ) {
            tmpch = rowhi[j*3];
            rowhi[j*3] = rowhi[j*3+2];
            rowhi[j*3+2] = tmpch;
            tmpch = rowlo[j*3];
            rowlo[j*3] = rowlo[j*3+2];
            rowlo[j*3+2] = tmpch;
        }
#endif
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
    return(image);
}


int Texture::get(const std::string & filename)
{
    if (textures().find(filename) != textures().end()) {
        return textures()[filename];
    }
    // std::cout << "Loading new texture " << filename << std::endl << std::flush;
    SDL_Surface * image = imageLoad(filename);

    if (image == NULL) {
        std::cerr << "Failed to load texture " << filename
                  << std::endl << std::flush;
        return -1;
    }

    int tex_id = loadTexture(image);
    if (tex_id != -1) { textures()[filename] = tex_id; }
    return tex_id;
}
    
int Texture::loadTexture(SDL_Surface * image)
{
    int tex_id;
    int format, fmt;
    int x, y;
    int bpp = image->format->BitsPerPixel;

    if(bpp != 24 && bpp != 32) {
        SDL_FreeSurface(image);
        std::cerr << "Failed to load texture: wrong format "
                  << std::endl << std::flush;
        return -1;
    }

    format = (bpp == 24) ? GL_RGB : GL_RGBA;
    fmt = (bpp == 24) ? 3 : 4;

    /* load the texture into OGL */
    glGenTextures(1, (unsigned int *)&tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, image->w, image->h, 0,
                 format, GL_UNSIGNED_BYTE, image->pixels);
    std::cout << image->w << " " << image->h << std::endl << std::flush;
    int glerror;
    if ((glerror = glGetError()) != 0) {
        std::cerr << "BANFG " << glerror << " " << bpp << std::endl << std::flush;
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return tex_id;
}

//////////////////////////////////////////////////////////////////////////////

#include <cmath>

std::map<std::string, Tile *> * Tile::tiledb = NULL;

Tile * Tile::get(const std::string & filename)
{
    if (tiles().find(filename) != tiles().end()) {
        return tiles()[filename];
    }
    // std::cout << "Loading new tile " << filename << std::endl << std::flush;
    Tile * t = new Tile();
    t->load(filename);

    if (!t->loaded()) {
        std::cerr << "Failed to load texture " << filename
                  << std::endl << std::flush;
        return NULL;
    }

    tiles()[filename] = t;
    return t;
}
    


unsigned int Tile::twoN(unsigned int size)
{
    for(int i = 0; i < 12; i++) {
        const unsigned int num = std::pow(2,i);
        if (size <= num) {
            return num;
        }
    }
    return std::pow(2,12);
}

bool Tile::load(const std::string & filename)
{
    // cout << "Tile " << filename << endl << flush;
    SDL_Surface * image = IMG_Load(filename.c_str());
    if (image == NULL) {
        return false;
    }

    const unsigned int textur_w = twoN(image->w);
    const unsigned int textur_h = twoN(image->h);
    const unsigned int sprite_w = image->w;
    unsigned int sprite_h = image->h;
    SDL_Surface * texImage = SDL_CreateRGBSurface(SDL_SWSURFACE, textur_w,
                                                  textur_h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                    0x000000FF, 
                    0x0000FF00, 
                    0x00FF0000, 
                    0xFF000000
#else
                    0xFF000000,
                    0x00FF0000, 
                    0x0000FF00, 
                    0x000000FF
#endif
                    );

    SDL_SetAlpha(image, 0, 0);
    SDL_Rect dest = { 0, textur_h - sprite_h, sprite_w, sprite_h };
    SDL_BlitSurface(image, NULL, texImage, &dest);
    SDL_FreeSurface(image);
    image = Texture::imageTransform(texImage);
    if (image == NULL) {
        SDL_FreeSurface(texImage);
        return false;
    }
    tex_id = Texture::loadTexture(image);
    //m_w = (float)sprite_w;
    //m_h = (float)sprite_h;
    if (sprite_w == 70) {
        tileSize = 1;
        if (sprite_h > 36) { sprite_h = 36; }
    } else if (sprite_w == 142) {
        tileSize = 2;
        if (sprite_h > 72) { sprite_h = 72; }
    } else if (sprite_w == 214) {
        tileSize = 3;
        if (sprite_h > 108) { sprite_h = 108; }
    } else if (sprite_w == 430) {
        tileSize = 6;
        if (sprite_h > 216) { sprite_h = 216; }
    } else {
        cout << "Tile size is unknown " << tileSize << "," << sprite_w << endl << flush;
        tileSize = 0;
    }
    m_pw = (float)sprite_w / textur_w;
    m_ph = (float)sprite_h / textur_h;
    SDL_FreeSurface(image);
    if (tex_id == -1) { return false; }
    return true;
}

void Tile::draw()
{
    if (tex_id == -1) { return; }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(m_pw/2, 0); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(m_pw, m_ph/2); glVertex3f(tileSize, 0.0f, 0.0f);
    glTexCoord2f(m_pw/2, m_ph); glVertex3f(tileSize, tileSize, 0.0f);
    glTexCoord2f(0, m_ph/2); glVertex3f(0.0f, tileSize, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

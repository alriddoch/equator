// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#error This file has been removed from the build

#include "Tile.h"

#include "Texture.h"
#include "GL.h"

#include <Mercator/Terrain.h>

#include <SDL_image.h>

#include <iostream>
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
        const unsigned int num = (1 << i);
        if (size <= num) {
            return num;
        }
    }
    return (1 << 12);
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
        std::cout << "Tile size is unknown " << filename << "," << sprite_w << std::endl << std::flush;
        tileSize = 0;
    }
    m_pw = (float)sprite_w / textur_w;
    m_ph = (float)sprite_h / textur_h;
    SDL_FreeSurface(image);
    if (tex_id == -1) { return false; }
    m_name = filename;
    return true;
}

void Tile::draw()
{
    if (tex_id == -1) { return; }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float texcoords[] = { m_pw / 2.f, 0.f,
                          m_pw, m_ph / 2.f,
                          m_pw / 2.f, m_ph,
                          0, m_ph / 2.f };
    float vertices[] = { 0.f, 0.f, 0.f,
                         tileSize, 0.f, 0.f,
                         tileSize, tileSize, 0.f,
                         0.f, tileSize, 0.f };
                        
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

void Tile::draw(const Mercator::Terrain & h, int x, int y)
{
    if (tex_id == -1) { return; }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float dw = m_pw / (2 * tileSize);
    float dh = m_ph / (2 * tileSize);
    int tile_size = tileSize + 1;
    GLfloat vertices[(tile_size * tile_size) * 3],
            texcoord[(tile_size * tile_size) * 3];
    int tindex = -1;
    int vindex = -1;
    for(int j = 0; j < tile_size; ++j) {
        for(int i = 0; i < tile_size; ++i) {
            texcoord[++tindex] = m_pw/2 + dw * (i - j);
            texcoord[++tindex] = dh * (i + j);
            vertices[++vindex] = i;
            vertices[++vindex] = j;
            vertices[++vindex] = h.get(x + i, y + j);
        }
    }
    GLuint indices[(int)tileSize * tile_size * 4];
    int iindex = -1;
    for (int i = 0; i < tile_size - 1; ++i) {
        for (int j = 0; j < tile_size; ++j) {
            indices[++iindex] = j * tile_size + i;
            indices[++iindex] = j * tile_size + i + 1;
        }
        if (++i >= tileSize) { break; }
        for (int j = tileSize; j >= 0; --j) {
            indices[++iindex] = j * tile_size + i + 1;
            indices[++iindex] = j * tile_size + i;
        }
    }
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoord);
    glDrawElements(GL_TRIANGLE_STRIP, ++iindex, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void Tile::outline(float offset)
{
    GLfloat vertices[] = { 0.f, 0.f, 0.f,
                           tileSize, 0.f, 0.f,
                           tileSize, tileSize, 0.f,
                           0.f, tileSize, 0.f,
                           0.f, 0.f, 0.f };

    GLfloat texcoords[] = { offset, offset + tileSize,
                            offset, offset + tileSize,
                            offset };
    glColor3f(0.0f, 0.0f, 0.5f);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(1, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_LINE_STRIP, 0, 5);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Tile::select()
{
    if (tex_id == -1) { return; }
    GLfloat vertices[] = { 0.f, 0.f, 0.f,
                           tileSize, 0.f, 0.f, 
                           tileSize, tileSize, 0.f,
                           0.f, tileSize, 0.f };
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef EQUATOR_VISUAL_STOCK_TEXTURE_H
#define EQUATOR_VISUAL_STOCK_TEXTURE_H

#include "GL.h"

enum StockTexture
{
  STOCK_HOLO = 1
};

GLuint getStockTexture(StockTexture);

#endif // EQUATOR_VISUAL_STOCK_TEXTURE_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "HeightMap.h"

#include "png.h"

#include <iostream>

#include <cassert>

void pngErrorFunction(png_structp, const char *c)
{
    std::cerr << "pngErrorFunction" << c << std::endl << std::flush;
}

void pngWarningFunction(png_structp, const char *c)
{
    std::cerr << "pngWarningFunction: " << c << std::endl << std::flush;
}

HeightMap::HeightMap() : m_rowPointers(NULL)
{
}

bool HeightMap::load(const std::string & file)
{
    FILE *img_fp = fopen(file.c_str(), "rb");
    if (!img_fp) {
        std::cerr << "Image file " << file << " could not be opened."
                  << std::endl << std::flush;
        perror("fopen");
        return false;
    }

    png_byte header[8];
    fread(header, 1, 8, img_fp);
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        std::cerr << "Image file " << file << " does not seem to be a png."
                  << std::endl << std::flush;
        return false;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 (png_voidp)NULL,
                                                 pngErrorFunction,
                                                 pngWarningFunction);
    if (!png_ptr) {
        std::cerr << "libpng failed to create read struct"
                  << std::endl << std::flush;
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cerr << "libpng failed to create info struct"
                  << std::endl << std::flush;
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return false;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        std::cerr << "libpng failed to create end info struct"
                  << std::endl << std::flush;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return false;
    }
    std::cout << "PNG_READ: Succeeded in getting this far"
              << std::endl << std::flush;
    png_init_io(png_ptr, img_fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);

    int depth, colourType;
    png_get_IHDR(png_ptr, info_ptr,
                 (png_uint_32*)&m_width, (png_uint_32*)&m_height, &depth,
                 &colourType, NULL, NULL, NULL);

    if ((depth != 8) || (colourType != PNG_COLOR_TYPE_GRAY)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return false;
    }

    m_rowPointers = png_get_rows(png_ptr, info_ptr);

    return true;
}

int HeightMap::get(int x, int y) const
{
    assert(m_rowPointers != NULL);
    
    return *(m_rowPointers[m_height - 1 - y] + x) - 127;
}

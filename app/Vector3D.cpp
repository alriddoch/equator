// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2002 Alistair Riddoch

#include <iostream>

#include "Vector3D.h"

std::ostream & operator<<(std::ostream& s, const Eris::Coord& v) {
    return s << "[" << v.x << "," << v.y << "," << v.z << "]";
}

std::ostream & operator<<(std::ostream& s, const Vector3D& v) {
    if (!v._set) {
        return s << "[NS]";
    }
    return s << "[" << v.x << "," << v.y << "," << v.z << "]";
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef VECTOR_3D_H
#define VECTOR_3D_H

namespace WFMath {
  template<const int dim> class Vector;
  template<const int dim> class Point;
  class Quaternion;
}

typedef WFMath::Vector<3> VelType;
typedef WFMath::Point<3> PosType;

#endif // VECTOR_3D_H

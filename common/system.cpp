// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "system.h"

const std::string getMediaPath()
{
#if defined(_WIN32)
    #error I dont know what to put here yet.
#else
    return std::string(SHAREDIR);
#endif
}

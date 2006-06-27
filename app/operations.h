// Equator Online RPG World Building Tool
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

// Forward declarations of all the operation classes used in equator. Use
// of this file reduces complexities in the header dependency tree, and speeds
// up compile time.

#include <Atlas/Objects/Operation/RootOperation.h>

namespace Atlas {
    namespace Objects {
        namespace Operation {
            class Login;
            class Logout;
            class Action;
            class Combine;
            class Create;
            class Delete;
            class Divide;
            class Imaginary;
            class Move;
            class Set;
            class Get;
            class Sight;
            class Sound;
            class Touch;
            class Talk;
            class Look;
            class Info;
            class Appearance;
            class Disappearance;
            class Error;
            class RootOperation;
        }
    }
}

using Atlas::Objects::Operation::RootOperation;

using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Combine;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Divide;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Touch;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Error;

#endif // COMMON_OPERATIONS_H

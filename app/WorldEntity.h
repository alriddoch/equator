// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_WORLDENTITY_H
#define EQUATOR_WORLDENTITY_H

#include <Eris/Entity.h>
#include <Eris/Factory.h>

namespace Eris {
  class TypeInfo;
}

class Server;

class TerrainEntity : public Eris::Entity
{
  public:
    TerrainEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class WEFactory : public Eris::Factory
{
  private:
    static Eris::TypeInfo * terrainType;
  public:
    Server & m_svr;

    explicit WEFactory(Server & a);
    
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
};

#endif // EQUATOR_WORLDENTITY_H

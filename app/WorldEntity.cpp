// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "WorldEntity.h"

#include "Model.h"
#include "Server.h"
#include "Terrain.h"

#include <Eris/World.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

using Atlas::Objects::Entity::GameEntity;

TerrainEntity::TerrainEntity(const GameEntity &ge,
                             Eris::World * w) : Eris::Entity(ge, w)
{
}

WEFactory::WEFactory(Server & s) : m_svr(s)
{
}

Eris::TypeInfo * WEFactory::terrainType = 0;

bool WEFactory::accept(const GameEntity&, Eris::World * w)
{
    if (terrainType == 0) {
        terrainType = w->getConnection()->getTypeService()->getTypeByName("world");
    }

    return true;
}

Eris::EntityPtr WEFactory::instantiate(const GameEntity & ge, Eris::World * w)
{
    std::cout << "ENTITY FACTORY: " << std::flush;
    Eris::Entity * e = 0;
    Eris::TypeInfoPtr type = w->getConnection()->getTypeService()->getTypeForAtlas(ge);
    if (type->safeIsA(terrainType)) {
        std::cout << "got Terrain Entity" << std::endl << std::flush;
        TerrainEntity * t = new TerrainEntity(ge,w);
        Model * m = m_svr.getModel();
        if (m != 0) {
            Terrain * layer = new Terrain(*m);
            m_svr.readTerrain(*layer, *t);
            m->addLayer(layer);
            t->m_terrain = layer;
        }
        e = t;
    } else {
        std::cout << "default" << std::endl << std::flush;
        e = new Eris::Entity(ge, w);
    }
    return e;
}

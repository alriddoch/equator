// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "WorldEntity.h"

#include "visual/Cal3dRenderer.h"
#include "visual/3dsRenderer.h"
#include "visual/TerrainRenderer.h"
#include "visual/ForestRenderer.h"

#include "common/configuration.h"

#include <Eris/World.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>

#include <varconf/Config.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <SDL/SDL.h>

#include <cassert>

using Atlas::Objects::Entity::GameEntity;

RenderableEntity::RenderableEntity(const GameEntity &ge,
                             Eris::World * w) : Eris::Entity(ge, w), m_drawer(0)
{
}

void RenderableEntity::constrainChild(RenderableEntity &, PosType &)
{
}

MovableEntity::MovableEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
    Moved.connect(SigC::slot(*this, &MovableEntity::movedSignal));
    updateTime = SDL_GetTicks();
}

void MovableEntity::movedSignal(const PosType &)
{
    updateTime = SDL_GetTicks();
}

AutonomousEntity::AutonomousEntity(const GameEntity &ge,
                             Eris::World * w) : MovableEntity(ge, w)
{
}

TerrainEntity::TerrainEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
}

void TerrainEntity::constrainChild(RenderableEntity & re, PosType & pos)
{
    if (m_drawer == 0) {
        std::cout << "Constraint, but no drawer" << std::endl << std::flush;
        return;
    }
    TerrainRenderer * tr = dynamic_cast<TerrainRenderer *>(m_drawer);
    if (tr == 0) {
        std::cout << "Drawer is not terrain" << std::endl << std::flush;
        return;
    }
    WFMath::Vector<3> n;
    tr->m_terrain.getHeightAndNormal(pos.x(), pos.y(), pos.z(), n);
}

TreeEntity::TreeEntity(const GameEntity &ge,
                             Eris::World * w) : RenderableEntity(ge, w)
{
}

RenderFactory::~RenderFactory()
{
}

WEFactory::WEFactory(Eris::TypeService & ts, Renderer & r) :
    autonomousType(ts.getTypeByName("character")),
    terrainType(ts.getTypeByName("world")),
    treeType(ts.getTypeByName("tree")),
    m_renderer(r)
{
    assert(autonomousType != 0);
    assert(terrainType != 0);
    assert(treeType != 0);

    const varconf::sec_map & cal3d_list = global_conf->getSection("cal3d");
    varconf::sec_map::const_iterator I = cal3d_list.begin();
    for(; I != cal3d_list.end(); ++I) {
        const std::string filename = I->second;
        RenderFactory * rf = new RendererFactory<Cal3dRenderer>(filename);
        Eris::TypeInfo * ti = ts.getTypeByName(I->first);
        assert(ti != 0);
        m_renderFactories.insert(std::make_pair(ti, rf));
    }

    const varconf::sec_map & m3ds_list = global_conf->getSection("3ds");
    I = m3ds_list.begin();
    for(; I != m3ds_list.end(); ++I) {
        const std::string filename = I->second;
        RenderFactory * rf = new RendererFactory<m3dsRenderer>(filename);
        Eris::TypeInfo * ti = ts.getTypeByName(I->first);
        assert(ti != 0);
        m_renderFactories.insert(std::make_pair(ti, rf));
    }

    const varconf::sec_map & forest_list = global_conf->getSection("forest");
    I = forest_list.begin();
    for(; I != forest_list.end(); ++I) {
        const std::string filename = I->second;
        RenderFactory * rf = new RendererFactory<ForestRenderer>(filename);
        Eris::TypeInfo * ti = ts.getTypeByName(I->first);
        assert(ti != 0);
        m_renderFactories.insert(std::make_pair(ti, rf));
    }

    RenderFactory * rf = new RendererFactory<TerrainRenderer>("");
    Eris::TypeInfo * ti = ts.getTypeByName("world");
    assert(ti != 0);
    m_renderFactories.insert(std::make_pair(ti, rf));

    // rf = new RendererFactory<ForestRenderer>("");
    // ti = ts.getTypeByName("forest");
    // assert(ti != 0);
    // m_renderFactories.insert(std::make_pair(ti, rf));
}

WEFactory::~WEFactory()
{
}

bool WEFactory::accept(const GameEntity&, Eris::World * w)
{
    return true;
}

Eris::EntityPtr WEFactory::instantiate(const GameEntity & ge, Eris::World * w)
{
    SigC::Slot0<void> slot;
    RenderableEntity * re = 0;

    Eris::TypeInfoPtr type = w->getConnection()->getTypeService()->getTypeForAtlas(ge);
    if (type->safeIsA(autonomousType)) {
        AutonomousEntity * ae = new AutonomousEntity(ge,w);
        slot = SigC::bind<AutonomousEntity *>(AutonomousEntityCreated.slot(),
                                              ae);
        re = ae;
    } else if (type->safeIsA(terrainType)) {
        TerrainEntity * te = new TerrainEntity(ge,w);
        slot = SigC::bind<TerrainEntity *>(TerrainEntityCreated.slot(), te);
        re = te;
    } else if (type->safeIsA(treeType)) {
        TreeEntity * te = new TreeEntity(ge,w);
        slot = SigC::bind<TreeEntity *>(TreeEntityCreated.slot(), te);
        re = te;
    } else {
        re = new RenderableEntity(ge, w);
        slot = SigC::bind<RenderableEntity *>(RenderableEntityCreated.slot(),
                                              re);
    }
    if (re->m_drawer == 0) {
        RendererMap::const_iterator I = m_renderFactories.find(type);
        if (I != m_renderFactories.end()) {
            re->m_drawer = I->second->newRenderer(m_renderer, *re);
        } else {
            // FIXME Ascend the type tree to try and find a rough match?
            re->m_drawer = new BBoxRenderer(m_renderer, *re);
        }
    }
    slot();
    return re;
}

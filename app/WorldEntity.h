// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_WORLDENTITY_H
#define APOGEE_WORLDENTITY_H

#include "common/Vector3D.h"

#include <Eris/Entity.h>
#include <Eris/Factory.h>

#include <sigc++/signal.h>

namespace Eris {
  class TypeInfo;
  class TypeService;
}

class EntityRenderer;

class RenderableEntity : public Eris::Entity
{
  public:
    EntityRenderer * m_drawer;

    RenderableEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);
    virtual void constrainChild(RenderableEntity &, PosType & pos);
};

class MovableEntity : public RenderableEntity
{
    float updateTime;
  public:
    void movedSignal(const PosType &);

    MovableEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

    const float getTime() {
        return updateTime;
    }
};

class AutonomousEntity : public MovableEntity
{
  public:
    AutonomousEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class TerrainEntity : public RenderableEntity
{
  public:
    TerrainEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

    virtual void constrainChild(RenderableEntity &, PosType & pos);
};

class TreeEntity : public RenderableEntity
{
  public:
    TreeEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *);

};

class Renderer;

class RenderFactory
{
  public:
    RenderFactory() { }
    virtual ~RenderFactory();

    virtual EntityRenderer * newRenderer(Renderer &, Eris::Entity &) const = 0;
};

template <class R>
class RendererFactory : public RenderFactory
{
  private:
    std::string m_filename;
  public:
    explicit RendererFactory(const std::string & fname) : m_filename(fname) { }

    EntityRenderer * newRenderer(Renderer & r, Eris::Entity & e) const {
        R * er = new R(r, e);
        er->load(m_filename);
        return er;
    }
};

class Renderer;

class WEFactory : public Eris::Factory
{
  public:
    typedef std::map<Eris::TypeInfo *, RenderFactory *> RendererMap;
  private:
    Eris::TypeInfo * autonomousType;
    Eris::TypeInfo * terrainType;
    Eris::TypeInfo * treeType;

    RendererMap m_renderFactories;
  public:
    Renderer & m_renderer;

    explicit WEFactory(Eris::TypeService &, Renderer &);
    virtual ~WEFactory();
    
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &, Eris::World *);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &, Eris::World *);

    SigC::Signal1<void, AutonomousEntity *> AutonomousEntityCreated;
    SigC::Signal1<void, TerrainEntity *> TerrainEntityCreated;
    SigC::Signal1<void, TreeEntity *> TreeEntityCreated;
    SigC::Signal1<void, RenderableEntity *> RenderableEntityCreated;
};

#endif // APOGEE_WORLDENTITY_H

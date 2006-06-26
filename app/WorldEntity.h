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

    RenderableEntity(const std::string & id, Eris::TypeInfo *, Eris::View *);
    virtual void constrainChild(RenderableEntity &, PosType & pos);
};

class MovableEntity : public RenderableEntity
{
    float updateTime;
  public:
    void movedSignal();

    MovableEntity(const std::string & id, Eris::TypeInfo *, Eris::View *);

    const float getTime() {
        return updateTime;
    }
};

class AutonomousEntity : public MovableEntity
{
  public:
    AutonomousEntity(const std::string & id, Eris::TypeInfo *, Eris::View *);

};

class TerrainEntity : public RenderableEntity
{
  public:
    TerrainEntity(const std::string & id, Eris::TypeInfo *, Eris::View *);

    virtual void constrainChild(RenderableEntity &, PosType & pos);
};

class TreeEntity : public RenderableEntity
{
  public:
    TreeEntity(const std::string & id, Eris::TypeInfo *, Eris::View *);

};

class Renderer;

class RenderFactory
{
  public:
    RenderFactory() { }
    virtual ~RenderFactory();

    virtual EntityRenderer * newRenderer(Renderer &, RenderableEntity &) const = 0;
};

template <class R>
class RendererFactory : public RenderFactory
{
  private:
    std::string m_filename;
  public:
    explicit RendererFactory(const std::string & fname) : m_filename(fname) { }

    EntityRenderer * newRenderer(Renderer & r, RenderableEntity & e) const {
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
    
    virtual bool accept(const Atlas::Objects::Entity::RootEntity &, Eris::TypeInfo *);
    virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::RootEntity &, Eris::TypeInfo *, Eris::View *);

    sigc::signal<void, AutonomousEntity *> AutonomousEntityCreated;
    sigc::signal<void, TerrainEntity *> TerrainEntityCreated;
    sigc::signal<void, TreeEntity *> TreeEntityCreated;
    sigc::signal<void, RenderableEntity *> RenderableEntityCreated;
};

#endif // APOGEE_WORLDENTITY_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Server.h"

#include "Model.h"
#include "WorldEntity.h"
#include "ServerEntities.h"
#include "Terrain.h"

#include <Eris/Avatar.h>
#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/PollDefault.h>

#include <Mercator/Terrain.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Create.h>

#include <glibmm/main.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Element;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::GameEntity;

#if 0
inline Atlas::Message::Element WFMath::Point<3>::toAtlas() const
{
    Atlas::Message::Element::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Element(ret);
}

inline Atlas::Message::Element WFMath::Vector<3>::toAtlas() const
{
    Atlas::Message::Element::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Element(ret);
}
#endif

Server::Server() : inGame(false),
                   m_connection(* new Eris::Connection("equator", true)),
                   m_player(NULL), m_lobby(NULL), m_world(NULL),
                   m_character(NULL)
{
}

void Server::lobbyTalk(Eris::Room *r, const std::string & nm,
                                      const std::string & t)
{
    std::cout << "TALK: " << t << std::endl << std::flush;
}

void Server::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    std::cout << "Logged in" << std::endl << std::flush;

    // FIXME Event triggered here to get tell the user what stage we
    // are at
}

void Server::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter(GameEntity::Instantiate());
    chrcter.setParents(Atlas::Message::Element::ListType(1,type));
    chrcter.setName(name);
    chrcter.setAttr("description", "an equator avatar");
    // chrcter.setAttr("sex", "female");
    m_world = m_player->createCharacter(chrcter)->getWorld();

    m_lobby->Talk.connect(SigC::slot(*this,&Server::lobbyTalk));
    m_lobby->Entered.connect(SigC::slot(*this,&Server::roomEnter));

    m_world->EntityCreate.connect(SigC::slot(*this,&Server::worldEntityCreate));
    m_world->Entered.connect(SigC::slot(*this,&Server::worldEnter));
    m_world->registerFactory(new WEFactory(*this));

}

void Server::readTerrain(Terrain & t, Eris::Entity & ent)
{
    if (!ent.hasProperty("terrain")) {
        std::cerr << "Terrain object has no terrain" << std::endl << std::flush;
        std::cerr << "Terrain " << ent.getID() << std::endl << std::flush;
        return;
    }
    const Element & terrain = ent.getProperty("terrain");
    if (!terrain.isMap()) {
        std::cerr << "Terrain is not a map" << std::endl << std::flush;
    }
    const Element::MapType & tmap = terrain.asMap();
    Element::MapType::const_iterator I = tmap.find("points");
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
    if ((I == tmap.end()) || !I->second.isList()) {
        std::cerr << "No terrain points" << std::endl << std::flush;
    }
    const Element::ListType & plist = I->second.asList();
    Element::ListType::const_iterator J = plist.begin();
    for(; J != plist.end(); ++J) {
        if (!J->isList()) {
            std::cout << "Non list in points" << std::endl << std::flush;
            continue;
        }
        const Element::ListType & point = J->asList();
        if (point.size() < 3) {
            std::cout << "point without 3 nums" << std::endl << std::flush;
            continue;
        }
        int x = (int)point[0].asNum();
        int y = (int)point[1].asNum();
        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
        Mercator::BasePoint bp(point[2].asNum());
        if (point.size() > 3) {
            bp.roughness()=point[3].asNum();
            if (point.size() > 4) {
                bp.falloff()=point[4].asNum();
            }
        }
        t.m_terrain.setBasePoint(x, y, bp);
    }
 
#if 0
    //============HACKED TO TEST MERCATOR

    const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0,0.0), 12.0);
    Mercator::LevelTerrainMod<WFMath::Ball<2> > mod2(10.0f, circ2);
    m_model.m_terrain.addMod(mod2);

    const WFMath::RotBox<2> rot(WFMath::Point<2>(-80.,-130.) ,
                                WFMath::Vector<2>(150.0,120.0),
                                WFMath::RotMatrix<2>().rotation(WFMath::Pi/4));
    Mercator::LevelTerrainMod<WFMath::RotBox<2> > mod3(10.0f, rot);
    m_model.m_terrain.addMod(mod3);

    //====================================
#endif


}

void Server::checkEntityForNewLayers(Eris::Entity & ent)
{
    if (typeid(ent) == typeid(TerrainEntity)) {
        TerrainEntity & tent = dynamic_cast<TerrainEntity &>(ent);
        std::cout << "Found a terrain entity"
                  << std::endl << std::flush;
        Terrain * layer = new Terrain(*m_model);
        readTerrain(*layer, ent);
        m_model->addLayer(layer);
        tent.m_terrain = layer;
    }
    
    int numEnts = ent.getNumMembers();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent.getMember(i);
        checkEntityForNewLayers(*e);
    }
}

void Server::createLayers(Model & model)
{
    m_model = &model;
    Layer * layer = new ServerEntities(model, *this);
    model.addLayer(layer);

    Eris::Entity * worldRoot = m_world->getRootEntity();

    checkEntityForNewLayers(*worldRoot);

}

void Server::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void Server::connect(const std::string & host, int port)
{
    m_connection.Failure.connect(SigC::slot(*this, &Server::netFailure));
    m_connection.Connected.connect(SigC::slot(*this, &Server::netConnected));
    m_connection.Disconnected.connect(SigC::slot(*this, &Server::netDisconnected));
    Eris::Logged.connect(SigC::slot(*this, &Server::connectionLog));

    Eris::setLogLevel(Eris::LOG_DEBUG);

    std::cout << host << ":" << port << std::endl << std::flush;
    m_connection.connect(host, port);
    // m_connection.connect("localhost", 6767);

#warning Handle the socket input FIXME
    inputHandler = Glib::signal_io().connect(slot(*this, &Server::poll),
                                             m_connection.getFileDescriptor(),
                                             Glib::IO_IN);
}

void Server::netFailure(const std::string & msg)
{
    inputHandler.disconnect();
    std::cout << "Got connection failure: " << msg << std::endl << std::flush;
}

void Server::connectionLog(Eris::LogLevel level, const std::string & msg)
{
    std::cout << "LOG: " << msg << std::endl << std::flush;
}

void Server::netConnected()
{
    std::cout << "Connected to server!" << std::endl << std::flush;

}

void Server::login(const std::string & name, const std::string & password)
{
    m_player = new Eris::Player(&m_connection);
    m_player->login(name, password);
    m_lobby = m_connection.getLobby();
    m_lobby->LoggedIn.connect(SigC::slot(*this, &Server::loginComplete));
}

void Server::createAccount(const std::string& name, const std::string& password)
{
    m_player = new Eris::Player(&m_connection);
    m_player->createAccount(name, name, password);
    m_lobby = m_connection.getLobby();
    m_lobby->LoggedIn.connect(SigC::slot(*this, &Server::loginComplete));
}

void Server::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
}

bool Server::poll(Glib::IOCondition)
{
    //m_connection.poll();
    Eris::PollDefault::poll();
    return true;
}

void Server::worldEntityCreate(Eris::Entity *r)
{
    std::cout << "Created character" << std::endl << std::flush;
}

void Server::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    chr->Moved.connect(SigC::slot(*this, &Server::charMoved));
    m_character = chr;

}

void Server::charMoved(const WFMath::Point<3> &)
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void Server::moveCharacter(const WFMath::Point<3> & pos)
{
    if (m_character == NULL) {
        return;
    }
    
    Move m(Move::Instantiate());

    Atlas::Message::Element::MapType marg;
    marg["id"] = m_character->getID();
    marg["loc"] = m_character->getContainer()->getID();
    marg["pos"] = pos.toAtlas();
    marg["velocity"] = WFMath::Point<3>(1,0,0).toAtlas();
    m.setArgs(Atlas::Message::Element::ListType(1, marg));
    m.setFrom(m_character->getID());

    m_connection.send(m);
    
}

const WFMath::Point<3> operator+(const WFMath::Point<3> & lhs,
                                 const WFMath::Point<3> & rhs)
{
    return WFMath::Point<3>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

const WFMath::Point<3> Server::getAbsCharPos()
{
    if (!inGame) {
        return WFMath::Point<3>();
    }
    WFMath::Point<3> pos = m_character->getPosition();
    Eris::Entity * root = m_world->getRootEntity();
    for(Eris::Entity * ref = m_character->getContainer();
        ref != NULL && ref != root;
        ref = ref->getContainer()) {
        pos = pos + ref->getPosition();
    }
    return pos;
}

void Server::avatarCreateEntity(const Atlas::Message::Element::MapType & ent)
{
    Create c = Create::Instantiate();

    c.setArgs(Atlas::Message::Element::ListType(1, ent));
    c.setFrom(m_character->getID());
    c.setTo(m_character->getID());
    
    m_connection.send(c);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const WFMath::Point<3> & pos)
{
    Move m = Move::Instantiate();

    Atlas::Message::Element::MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    m.setArgs(Atlas::Message::Element::ListType(1, ent));
    m.setFrom(m_character->getID());
    m.setTo(id);

    m_connection.send(m);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const WFMath::Point<3> & pos,
                              const WFMath::Vector<3> & vel)
{
    Move m = Move::Instantiate();

    Atlas::Message::Element::MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    ent["velocity"] = vel.toAtlas();
    m.setArgs(Atlas::Message::Element::ListType(1, ent));
    m.setFrom(m_character->getID());
    m.setTo(id);

    m_connection.send(m);
}

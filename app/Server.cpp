// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Server.h"

#include "Model.h"
#include "WorldEntity.h"
#include "ServerEntities.h"
#include "GameView.h"
#include "Terrain.h"
#include "MainWindow.h"

#include "visual/TerrainRenderer.h"
#include "visual/Renderer.h"

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

#include <sigc++/object_slot.h>

#include <sstream>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::GameEntity;

unsigned Server::serverCount = 0;

Server::Server(MainWindow & mw, const std::string & name) :
                   m_serverNo(serverCount++),
                   inGame(false), m_name(name), m_model(0),
                   m_mainWindow(mw),
                   m_renderer(* new Renderer),
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

void Server::loginComplete()
{
    std::cout << "Logged in" << std::endl << std::flush;

    // FIXME Event triggered here to get tell the user what stage we
    // are at
}

void Server::connectWorldSignals()
{
    m_lobby->Talk.connect(SigC::slot(*this,&Server::lobbyTalk));
    m_lobby->Entered.connect(SigC::slot(*this,&Server::roomEnter));

    m_world->EntityCreate.connect(SigC::slot(*this,&Server::worldEntityCreate));
    m_world->CharacterSuccess.connect(SigC::slot(*this,&Server::gotAvatar));
    m_world->Entered.connect(SigC::slot(*this,&Server::worldEnter));

    WEFactory * wef = new WEFactory(*m_connection.getTypeService(), m_renderer);
    wef->TerrainEntityCreated.connect(SigC::slot(*this, &Server::createTerrainLayer));
    m_world->registerFactory(wef);
}

void Server::takeCharacter(const std::string & id)
{
    m_world = m_player->takeCharacter(id)->getWorld();

    connectWorldSignals();
}

void Server::createCharacter(const std::string & name,
                                 const std::string & type)
{

    GameEntity chrcter;
    chrcter.setParents(ListType(1,type));
    chrcter.setName(name);
    chrcter.setAttr("description", "an equator avatar");
    // chrcter.setAttr("sex", "female");
    m_world = m_player->createCharacter(chrcter)->getWorld();

    connectWorldSignals();
}

void Server::checkEntityForNewLayers(Eris::Entity & ent)
{
    assert(m_model != 0);

    if (typeid(ent) == typeid(TerrainEntity)) {
        TerrainEntity & tent = dynamic_cast<TerrainEntity &>(ent);
        std::cout << "Found a terrain entity"
                  << std::endl << std::flush;
        createTerrainLayer(&tent);
    }
    
    int numEnts = ent.getNumMembers();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent.getMember(i);
        checkEntityForNewLayers(*e);
    }
}

void Server::takeModel(Model & model)
{
    m_model = &model;
}

void Server::createLayers()
{
    assert(m_model != 0);

    Layer * layer = new ServerEntities(*m_model, *this);
    m_model->addLayer(layer);

    layer = new GameView(*m_model, *this);
    m_model->addLayer(layer);

    Eris::Entity * worldRoot = m_world->getRootEntity();

    checkEntityForNewLayers(*worldRoot);

}

void Server::createTerrainLayer(TerrainEntity * tent)
{
    assert(m_model != 0);

    EntityRenderer * er = tent->m_drawer;
    if (er != 0) {
        TerrainRenderer * tr = dynamic_cast<TerrainRenderer *>(er);
        if (tr != 0) {
            Terrain * layer = new Terrain(*m_model, tr->m_terrain);
            // readTerrain(*layer, ent); FIXME Perhaps we should do this here?
            layer->readTerrain(*tent);
            layer->TerrainModified.connect(SigC::bind<Terrain*, TerrainEntity *>(SigC::slot(*this, &Server::modifyTerrain), layer, tent));
            m_model->addLayer(layer);
        }
    }
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
    m_player->LoginSuccess.connect(SigC::slot(*this, &Server::loginComplete));
    m_lobby = m_connection.getLobby();
}

void Server::createAccount(const std::string& name, const std::string& password)
{
    m_player = new Eris::Player(&m_connection);
    m_player->createAccount(name, name, password);
    m_player->LoginSuccess.connect(SigC::slot(*this, &Server::loginComplete));
    m_lobby = m_connection.getLobby();
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

void Server::gotAvatar()
{
    Model & model = m_mainWindow.newModel();
    model.setName(getName());
    takeModel(model);
}

void Server::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    m_character = chr;

    createLayers();

    chr->Moved.connect(SigC::slot(*this, &Server::charMoved));
}

void Server::charMoved(const PosType &)
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void Server::moveCharacter(const PosType & pos)
{
    if (m_character == NULL) {
        return;
    }
    
    Move m;

    MapType marg;
    marg["id"] = m_character->getID();
    marg["loc"] = m_character->getContainer()->getID();
    marg["pos"] = pos.toAtlas();
    marg["velocity"] = VelType(1,0,0).toAtlas();
    m.setArgs(ListType(1, marg));
    m.setFrom(m_character->getID());

    m_connection.send(m);
    
}

void Server::modifyTerrain(Terrain * layer, TerrainEntity * tent)
{
    std::cerr << "Server::modifyTerrain()" << std::endl << std::flush;
    if (m_character == NULL) {
        return;
    }
    
    Set s;

    MapType sarg;
    sarg["id"] = tent->getID();
    MapType & terrain = (sarg["terrain"] = MapType()).asMap();

    Element::MapType & pointMap = (terrain["points"] = Element::MapType()).asMap();

    const Mercator::Terrain::Pointstore & points = layer->m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator I = points.begin();
    for(; I != points.end(); ++I) {
        const Mercator::Terrain::Pointcolumn & pointcol = I->second;
        Mercator::Terrain::Pointcolumn::const_iterator J = pointcol.begin();
        for (; J != pointcol.end(); ++J) {
            std::stringstream key;
            key << I->first << "x" << J->first;
            Element::ListType & point =
                    (pointMap[key.str()] = Element::ListType(3)).asList();
            point[0] = (Element::FloatType)(I->first);
            point[1] = (Element::FloatType)(J->first);
            point[2] = (Element::FloatType)(J->second.height());
        }
    }

    s.setArgs(ListType(1, sarg));
    s.setFrom(m_character->getID());

    m_connection.send(s);
    std::cerr << "Sent set op to server" << std::endl << std::flush;
}

const PosType Server::getAbsCharPos()
{
    if (!inGame) {
        return PosType();
    }
    PosType pos = m_character->getPosition();
    Eris::Entity * root = m_world->getRootEntity();
    for(Eris::Entity * ref = m_character->getContainer();
        ref != NULL && ref != root;
        ref = ref->getContainer()) {
        pos = pos.toParentCoords(ref->getPosition());
    }
    return pos;
}

void Server::avatarCreateEntity(const MapType & ent)
{
    Create c;

    c.setArgs(ListType(1, ent));
    c.setFrom(m_character->getID());
    c.setTo(m_character->getID());
    
    m_connection.send(c);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const PosType & pos)
{
    Move m;

    MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    m.setArgs(ListType(1, ent));
    m.setFrom(m_character->getID());
    m.setTo(id);

    m_connection.send(m);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const PosType & pos,
                              const VelType & vel)
{
    Move m;

    MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    ent["velocity"] = vel.toAtlas();
    m.setArgs(ListType(1, ent));
    m.setFrom(m_character->getID());
    m.setTo(id);

    m_connection.send(m);
}

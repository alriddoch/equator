// Equator Online RPG World Building Tool
// Copyright (C) 2000-2001 Alistair Riddoch
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

#include "Server.h"

#include "Holo.h"
#include "Model.h"
#include "WorldEntity.h"
#include "ServerEntities.h"
#include "GameView.h"
#include "Terrain.h"
#include "MainWindow.h"

#include "visual/TerrainRenderer.h"
#include "visual/Renderer.h"

#include <Eris/View.h>
#include <Eris/Avatar.h>
#include <Eris/Account.h>
#include <Eris/Lobby.h>
#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/PollDefault.h>

#include <Mercator/Terrain.h>

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>

#include <sstream>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::GameEntity;

unsigned Server::serverCount = 0;

Server::Server(MainWindow & mw, const std::string & name) :
                   m_serverNo(serverCount++),
                   inGame(false), worldSignalsConnected(false),
                   m_name(name), m_model(0),
                   m_mainWindow(mw),
                   m_renderer(* new Renderer),
                   m_connection(0), m_account(0), m_avatar(0), m_lobby(0),
                   m_view(0), m_character(0)
{
}

Server::~Server()
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
    m_lobby = new Eris::Lobby(m_account);

    // FIXME Event triggered here to get tell the user what stage we
    // are at
}

void Server::connectWorldSignals()
{
    if (worldSignalsConnected) {
        return;
    }
    // m_lobby->Talk.connect(sigc::mem_fun(*this,&Server::lobbyTalk));
    // m_lobby->Entered.connect(sigc::mem_fun(*this,&Server::roomEnter));

    m_account->AvatarSuccess.connect(sigc::mem_fun(*this,&Server::gotAvatar));


    worldSignalsConnected = true;
}

void Server::takeCharacter(const std::string & id)
{
    m_account->takeCharacter(id);

    connectWorldSignals();
}

void Server::createCharacter(const std::string & name, const std::string & type)
{

    GameEntity chrcter;
    chrcter->setParents(std::list<std::string>(1,type));
    chrcter->setName(name);
    chrcter->setAttr("description", "an equator avatar");
    // chrcter.setAttr("sex", "female");
    m_account->createCharacter(chrcter);

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
    
    int numEnts = ent.numContained();
    for (int i = 0; i < numEnts; i++) {
        Eris::Entity * e = ent.getContained(i);
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

    // layer = new GameView(*m_model, *this);
    // m_model->addLayer(layer);

    Eris::Entity * worldRoot = m_view->getTopLevel();

    if (worldRoot != 0) {
        checkEntityForNewLayers(*worldRoot);
    }
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
            layer->TerrainModified.connect(sigc::bind<Terrain*, TerrainEntity *>(sigc::mem_fun(*this, &Server::modifyTerrain), layer, tent));
            m_model->addLayer(layer);
        }
    }
}

void Server::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

bool Server::isConnected() const
{
    return ((m_connection != 0) && (m_connection->isConnected() == true));
}

bool Server::isLoggedIn() const
{
    return ((m_account != 0) && (m_account->isLoggedIn() == true));
}

void Server::setupServerConnection(const std::string & host, int port)
{
    std::cout << host << ":" << port << std::endl << std::flush;

    m_connection = new Eris::Connection("equator", host, port, true);

    m_connection->Failure.connect(sigc::mem_fun(*this, &Server::netFailure));
    m_connection->Connected.connect(sigc::mem_fun(*this, &Server::netConnected));
    m_connection->Disconnected.connect(sigc::mem_fun(*this, &Server::netDisconnected));

    Eris::Logged.connect(sigc::mem_fun(*this, &Server::connectionLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);
}

void Server::connect()
{
    m_connection->connect();

    inputHandler = Glib::signal_io().connect(sigc::mem_fun(*this, &Server::poll),
                                             m_connection->getFileDescriptor(),
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
    m_account = new Eris::Account(m_connection);
    m_account->login(name, password);
    m_account->LoginSuccess.connect(sigc::mem_fun(*this, &Server::loginComplete));
}

void Server::createAccount(const std::string& name, const std::string& password)
{
    m_account = new Eris::Account(m_connection);
    m_account->createAccount(name, name, password);
    m_account->LoginSuccess.connect(sigc::mem_fun(*this, &Server::loginComplete));
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

void Server::gotAvatar(Eris::Avatar * av)
{
    // FIXME Is it necessary to store the Avatar pointer
    m_avatar = av;
    m_avatar->GotCharacterEntity.connect(sigc::mem_fun(*this,&Server::worldEnter));

    m_view = m_avatar->getView();
    m_view->EntityCreated.connect(sigc::mem_fun(*this,&Server::worldEntityCreate));

    WEFactory * wef = new WEFactory(*m_connection->getTypeService(), m_renderer);
    wef->TerrainEntityCreated.connect(sigc::mem_fun(*this, &Server::createTerrainLayer));
    m_view->registerFactory(wef);

    Model & model = m_mainWindow.newModel();
    model.setName(getName());
    model.getBackDropLayer()->setBoxSize(0);
    takeModel(model);
}

void Server::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    m_character = chr;

    createLayers();

    chr->Moved.connect(sigc::mem_fun(*this, &Server::charMoved));
}

void Server::charMoved()
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
    marg["id"] = m_character->getId();
    marg["loc"] = m_character->getLocation()->getId();
    marg["pos"] = pos.toAtlas();
    marg["velocity"] = VelType(1,0,0).toAtlas();
    m->setArgsAsList(ListType(1, marg));
    m->setFrom(m_character->getId());

    m_connection->send(m);
    
}

void Server::modifyTerrain(Terrain * layer, TerrainEntity * tent)
{
    std::cerr << "Server::modifyTerrain()" << std::endl << std::flush;
    if (m_character == NULL) {
        return;
    }
    
    Set s;

    MapType sarg;
    sarg["id"] = tent->getId();
    MapType & terrain = (sarg["terrain"] = MapType()).asMap();

    MapType & pointMap = (terrain["points"] = MapType()).asMap();

    const Mercator::Terrain::Pointstore & points = layer->m_terrain.getPoints();
    Mercator::Terrain::Pointstore::const_iterator I = points.begin();
    for(; I != points.end(); ++I) {
        const Mercator::Terrain::Pointcolumn & pointcol = I->second;
        Mercator::Terrain::Pointcolumn::const_iterator J = pointcol.begin();
        for (; J != pointcol.end(); ++J) {
            std::stringstream key;
            key << I->first << "x" << J->first;
            ListType & point =
                    (pointMap[key.str()] = ListType(3)).asList();
            point[0] = (FloatType)(I->first);
            point[1] = (FloatType)(J->first);
            point[2] = (FloatType)(J->second.height());
        }
    }

    s->setArgsAsList(ListType(1, sarg));
    s->setFrom(m_character->getId());

    m_connection->send(s);
    std::cerr << "Sent set op to server" << std::endl << std::flush;
}

const PosType Server::getAbsCharPos()
{
    if (!inGame) {
        return PosType();
    }
    PosType pos = m_character->getPosition();
    Eris::Entity * root = m_view->getTopLevel();
    Eris::Entity * ref = m_character->getLocation();
    for(; ref != NULL && ref != root; ref = ref->getLocation()) {
        WFMath::Quaternion q = ref->getOrientation();
        if (!q.isValid()) {
            q.identity();
        }
        pos = pos.toParentCoords(ref->getPosition(), q);
    }
    return pos;
}

void Server::avatarCreateEntity(const MapType & ent)
{
    Create c;

    c->setArgsAsList(ListType(1, ent));
    c->setFrom(m_character->getId());
    c->setTo(m_character->getId());
    
    m_connection->send(c);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const PosType & pos)
{
    Move m;

    MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    m->setArgsAsList(ListType(1, ent));
    m->setFrom(m_character->getId());
    m->setTo(id);

    m_connection->send(m);
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
    m->setArgsAsList(ListType(1, ent));
    m->setFrom(m_character->getId());
    m->setTo(id);

    m_connection->send(m);
}

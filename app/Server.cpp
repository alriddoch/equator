// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Server.h"

#include "WorldEntity.h"

#include <Eris/Avatar.h>
#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/PollDefault.h>

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
                   connection(* new Eris::Connection("equator", true)),
                   player(NULL), lobby(NULL), world(NULL), character(NULL) { }

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
    world = player->createCharacter(chrcter)->getWorld();

    lobby->Talk.connect(SigC::slot(*this,&Server::lobbyTalk));
    lobby->Entered.connect(SigC::slot(*this,&Server::roomEnter));

    world->EntityCreate.connect(SigC::slot(*this,&Server::worldEntityCreate));
    world->Entered.connect(SigC::slot(*this,&Server::worldEnter));
    world->registerFactory(new WEFactory(*this));

}

void Server::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void Server::connect(const std::string & host, int port)
{
    connection.Failure.connect(SigC::slot(*this, &Server::netFailure));
    connection.Connected.connect(SigC::slot(*this, &Server::netConnected));
    connection.Disconnected.connect(SigC::slot(*this, &Server::netDisconnected));
    Eris::Logged.connect(SigC::slot(*this, &Server::connectionLog));

    Eris::setLogLevel(Eris::LOG_DEBUG);

    std::cout << host << ":" << port << std::endl << std::flush;
    connection.connect(host, port);
    // connection.connect("localhost", 6767);

#warning Handle the socket input FIXME
    inputHandler = Glib::signal_io().connect(slot(*this, &Server::poll),
                                             connection.getFileDescriptor(),
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
    player = new Eris::Player(&connection);
    player->login(name, password);
    lobby = connection.getLobby();
    lobby->LoggedIn.connect(SigC::slot(*this, &Server::loginComplete));
}

void Server::createAccount(const std::string& name, const std::string& password)
{
    player = new Eris::Player(&connection);
    player->createAccount(name, name, password);
    lobby = connection.getLobby();
    lobby->LoggedIn.connect(SigC::slot(*this, &Server::loginComplete));
}

void Server::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
}

bool Server::poll(Glib::IOCondition)
{
    //connection.poll();
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
    character = chr;

}

void Server::charMoved(const WFMath::Point<3> &)
{
    std::cout << "Char moved" << std::endl << std::flush;
}

void Server::moveCharacter(const WFMath::Point<3> & pos)
{
    if (character == NULL) {
        return;
    }
    
    Move m(Move::Instantiate());

    Atlas::Message::Element::MapType marg;
    marg["id"] = character->getID();
    marg["loc"] = character->getContainer()->getID();
    marg["pos"] = pos.toAtlas();
    marg["velocity"] = WFMath::Point<3>(1,0,0).toAtlas();
    m.setArgs(Atlas::Message::Element::ListType(1, marg));
    m.setFrom(character->getID());

    connection.send(m);
    
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
    WFMath::Point<3> pos = character->getPosition();
    Eris::Entity * root = world->getRootEntity();
    for(Eris::Entity * ref = character->getContainer();
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
    c.setFrom(character->getID());
    c.setTo(character->getID());
    
    connection.send(c);
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
    m.setFrom(character->getID());
    m.setTo(id);

    connection.send(m);
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
    m.setFrom(character->getID());
    m.setTo(id);

    connection.send(m);
}

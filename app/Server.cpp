// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#include "Server.h"

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Entity.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Create.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Object;

using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::GameEntity;

inline Atlas::Message::Object WFMath::Point<3>::toAtlas() const
{
    Atlas::Message::Object::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Object(ret);
}

inline Atlas::Message::Object WFMath::Vector<3>::toAtlas() const
{
    Atlas::Message::Object::ListType ret(3);
    ret[0] = m_elem[0];
    ret[1] = m_elem[1];
    ret[2] = m_elem[2];
    return Atlas::Message::Object(ret);
}

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
    chrcter.SetParents(Atlas::Message::Object::ListType(1,type));
    chrcter.SetName(name);
    chrcter.SetAttr("description", "an equator avatar");
    // chrcter.SetAttr("sex", "female");
    world = player->createCharacter(chrcter);

    lobby->Talk.connect(SigC::slot(this,&Server::lobbyTalk));
    lobby->Entered.connect(SigC::slot(this,&Server::roomEnter));

    world->EntityCreate.connect(SigC::slot(this,&Server::worldEntityCreate));
    world->Entered.connect(SigC::slot(this,&Server::worldEnter));

}

void Server::roomEnter(Eris::Room *r)
{
    std::cout << "Enter room" << std::endl << std::flush;
}

void Server::connect(const std::string & host, int port)
{
    connection.Failure.connect(SigC::slot(this, &Server::netFailure));
    connection.Connected.connect(SigC::slot(this, &Server::netConnected));
    connection.Disconnected.connect(SigC::slot(this, &Server::netDisconnected));
    connection.Log.connect(SigC::slot(this, &Server::connectionLog));

    std::cout << host << ":" << port << std::endl << std::flush;
    connection.connect(host, port);
    // connection.connect("localhost", 6767);

    inputHandler = Gtk::Main::input.connect(slot(this, &Server::poll),
                                            connection.getFileDescriptor(),
                                            GDK_INPUT_READ);
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
    lobby = Eris::Lobby::instance();
    lobby->LoggedIn.connect(SigC::slot(this, &Server::loginComplete));
}

void Server::createAccount(const std::string& name, const std::string& password)
{
    player = new Eris::Player(&connection);
    player->createAccount(name, name, password);
    lobby = Eris::Lobby::instance();
    lobby->LoggedIn.connect(SigC::slot(this, &Server::loginComplete));
}

void Server::netDisconnected()
{
    std::cout << "Disconnected from the server" << std::endl << std::flush;
}

void Server::poll(int, GdkInputCondition)
{
    connection.poll();
}

void Server::worldEntityCreate(Eris::Entity *r)
{
    std::cout << "Created character" << std::endl << std::flush;
}

void Server::worldEnter(Eris::Entity * chr)
{
    std::cout << "Enter world" << std::endl << std::flush;
    inGame = true;
    chr->Moved.connect(SigC::slot(this, &Server::charMoved));
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

    Atlas::Message::Object::MapType marg;
    marg["id"] = character->getID();
    marg["loc"] = character->getContainer()->getID();
    marg["pos"] = pos.toAtlas();
    marg["velocity"] = WFMath::Point<3>(1,0,0).toAtlas();
    m.SetArgs(Atlas::Message::Object::ListType(1, marg));
    m.SetFrom(character->getID());

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

void Server::avatarCreateEntity(const Atlas::Message::Object::MapType & ent)
{
    Create c = Create::Instantiate();

    c.SetArgs(Atlas::Message::Object::ListType(1, ent));
    c.SetFrom(character->getID());
    
    connection.send(c);
}

void Server::avatarMoveEntity(const std::string & id, const std::string &loc,
                              const WFMath::Point<3> & pos,
                              const WFMath::Vector<3> & vel)
{
    Move m = Move::Instantiate();

    Atlas::Message::Object::MapType ent;
    ent["id"] = id;
    ent["pos"] = pos.toAtlas();
    ent["loc"] = loc;
    ent["velocity"] = vel.toAtlas();
    m.SetArgs(Atlas::Message::Object::ListType(1, ent));
    m.SetFrom(character->getID());
    m.SetTo(id);

    connection.send(m);
}

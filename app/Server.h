// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_SERVER_H
#define EQUATOR_SERVER_H

#include "Vector3D.h"

#include <Eris/Connection.h>
#include <Eris/Log.h>

#include <gtk--/main.h>

namespace Eris {
  class Player;
  class Lobby;
  class World;
  class Room;
  class Entity;
}

namespace WFMath {
  template<const int dim> class Point;
}

namespace Atlas {
 namespace Objects {
  namespace Entity {
   class Player;
  }
 }
}

class Vector3D;

class Server : public SigC::Object {
  private:
    void lobbyTalk(Eris::Room *r, const std::string& nm, const std::string& t);
    void loginComplete(const Atlas::Objects::Entity::Player &p);

    void roomEnter(Eris::Room *r);

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    void charMoved(const WFMath::Point<3> &);
  protected:
    bool inGame;
  public:
    Server();

    Eris::Connection & connection;
    Eris::Player * player;
    Eris::Lobby * lobby;
    Eris::World * world;
    Eris::Entity * character;

    SigC::Connection inputHandler;

    void connect(const std::string &, int);
    void netConnected();
    void netFailure(const std::string & msg);
    void connectionLog(Eris::LogLevel level, const std::string & msg);
    void netDisconnected();

    void poll(int, GdkInputCondition);

    void login(const std::string &, const std::string &);
    void createAccount(const std::string &, const std::string &);
    void createCharacter(const std::string &, const std::string &);

    void moveCharacter(const WFMath::Point<3> & pos);

    void avatarCreateEntity(const Atlas::Message::Object::MapType &);
    void avatarMoveEntity(const std::string &, const std::string &loc,
                          const WFMath::Point<3> & pos,
                          const WFMath::Vector<3> & vel = WFMath::Vector<3>());

    const WFMath::Point<3> getAbsCharPos();
};

#endif // EQUATOR_SERVER_H

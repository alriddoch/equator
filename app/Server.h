// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_SERVER_H
#define EQUATOR_SERVER_H

#include <Eris/Connection.h>
#include <Eris/Log.h>

#include <gtkmm/main.h>

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

class Model;
class Terrain;

class Server : public SigC::Object {
  private:
    void lobbyTalk(Eris::Room *r, const std::string& nm, const std::string& t);
    void loginComplete(const Atlas::Objects::Entity::Player &p);

    void roomEnter(Eris::Room *r);

    void worldEntityCreate(Eris::Entity *r);
    void worldEnter(Eris::Entity *r);

    void charMoved(const WFMath::Point<3> &);

    void checkEntityForNewLayers(Eris::Entity & ent);
    void readTerrain(Terrain & t, Eris::Entity & ent);
  protected:
    bool inGame;

    Model * m_model;

  public:
    Server();

    Eris::Connection & m_connection;
    Eris::Player * m_player;
    Eris::Lobby * m_lobby;
    Eris::World * m_world;
    Eris::Entity * m_character;

    SigC::Connection inputHandler;

    void connect(const std::string &, int);
    void netConnected();
    void netFailure(const std::string & msg);
    void connectionLog(Eris::LogLevel level, const std::string & msg);
    void netDisconnected();

    bool poll(Glib::IOCondition);

    void login(const std::string &, const std::string &);
    void createAccount(const std::string &, const std::string &);
    void takeCharacter(const std::string &);
    void createCharacter(const std::string &, const std::string &);
    void createLayers(Model & model);

    void moveCharacter(const WFMath::Point<3> & pos);

    void avatarCreateEntity(const Atlas::Message::Element::MapType &);
    void avatarMoveEntity(const std::string &, const std::string &loc,
                          const WFMath::Point<3> & pos);
    void avatarMoveEntity(const std::string &, const std::string &loc,
                          const WFMath::Point<3> & pos,
                          const WFMath::Vector<3> & vel);

    const WFMath::Point<3> getAbsCharPos();
};

#endif // EQUATOR_SERVER_H

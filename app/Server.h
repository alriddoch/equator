// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_SERVER_H
#define EQUATOR_SERVER_H

#include "common/Vector3D.h"

#include <Eris/Log.h>

#include <Atlas/Message/Element.h>

#include <glibmm/main.h>

namespace Eris {
  class Connection;
  class Account;
  class Avatar;
  class Lobby;
  class View;
  class Room;
  class Entity;
}

namespace Atlas {
  namespace Message {
    class Element;
  }
}

class MainWindow;
class Model;
class Renderer;
class TerrainEntity;
class Terrain;

class Server : public SigC::Object {
  private:
    static unsigned serverCount;
    const unsigned m_serverNo;
 
    void lobbyTalk(Eris::Room *r, const std::string& nm, const std::string& t);
    void loginComplete();

    void roomEnter(Eris::Room *r);

    void worldEntityCreate(Eris::Entity *r);
    void gotAvatar(Eris::Avatar *);
    void worldEnter(Eris::Entity *r);

    void charMoved(Eris::Entity *);

    void checkEntityForNewLayers(Eris::Entity & ent);

    void connectWorldSignals();
  protected:
    bool inGame;
    const std::string m_name;

    Model * m_model;

  public:
    explicit Server(MainWindow &, const std::string &);

    MainWindow & m_mainWindow;
    Renderer & m_renderer;

    Eris::Connection & m_connection;
    Eris::Account * m_account;
    Eris::Avatar * m_avatar;
    Eris::Lobby * m_lobby;
    Eris::View * m_view;
    Eris::Entity * m_character;

    SigC::Connection inputHandler;

    const unsigned getServerNo() const {
        return m_serverNo;
    }

    const std::string & getName() const {
        return m_name;
    }

    Model * getModel() const {
        return m_model;
    }

    bool isInGame() const {
        return inGame;
    }

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
    void takeModel(Model & model);
    void createLayers();
    void createTerrainLayer(TerrainEntity *);

    void moveCharacter(const PosType & pos);
    void modifyTerrain(Terrain * layer, TerrainEntity * entity);

    void avatarCreateEntity(const Atlas::Message::MapType &);
    void avatarMoveEntity(const std::string &, const std::string &loc,
                          const PosType & pos);
    void avatarMoveEntity(const std::string &, const std::string &loc,
                          const PosType & pos,
                          const VelType & vel);

    const PosType getAbsCharPos();
};

#endif // EQUATOR_SERVER_H

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

#ifndef EQUATOR_SERVER_H
#define EQUATOR_SERVER_H

#include "common/Vector3D.h"

#include <Eris/Log.h>

#include <Atlas/Message/Element.h>

#include <glibmm/main.h>

#include <sigc++/trackable.h>

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

class Server : public sigc::trackable {
  private:
    static unsigned serverCount;
    const unsigned m_serverNo;
 
    void lobbyTalk(Eris::Room *r, const std::string& nm, const std::string& t);
    void loginComplete();

    void roomEnter(Eris::Room *r);

    void worldEntityCreate(Eris::Entity *r);
    void gotAvatar(Eris::Avatar *);
    void worldEnter(Eris::Entity *r);

    void charMoved();

    void checkEntityForNewLayers(Eris::Entity & ent);

    void connectWorldSignals();
  protected:
    bool inGame;
    bool worldSignalsConnected;
    const std::string m_name;

    Model * m_model;

  public:
    explicit Server(MainWindow &, const std::string &);
    ~Server();

    MainWindow & m_mainWindow;
    Renderer & m_renderer;

    Eris::Connection * m_connection;
    Eris::Account * m_account;
    Eris::Avatar * m_avatar;
    Eris::Lobby * m_lobby;
    Eris::View * m_view;
    Eris::Entity * m_character;

    sigc::connection inputHandler;

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

    bool isConnected() const;
    bool isLoggedIn() const;
    void setupServerConnection(const std::string &, int);
    void connect();
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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Sniff.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>

#include <Atlas/Objects/Entity/Game.h>
#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Entity/Admin.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include "inheritance.h"

using Atlas::Message::Element;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Perception;
using Atlas::Objects::Operation::Communicate;
using Atlas::Objects::Operation::Perceive;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Smell;
using Atlas::Objects::Operation::Feel;
using Atlas::Objects::Operation::Listen;
using Atlas::Objects::Operation::Sniff;

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Entity::Account;
using Atlas::Objects::Entity::Player;
using Atlas::Objects::Entity::Admin;
using Atlas::Objects::Entity::Game;
using Atlas::Objects::Entity::GameEntity;

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
    Root * obj;
    Element::ListType children;

    obj = new Root();
    children.push_back("root_operation");
    children.push_back("root_entity");
    obj->setAttr("children", children);
    atlasObjects["root"] = obj;

    obj = new RootOperation();
    children = Element::ListType();
    children.push_back("action");
    children.push_back("info");
    obj->setAttr("children", children);
    atlasObjects["root_operation"] = obj;

    obj = new Action();
    children = Element::ListType();
    children.push_back("create");
    children.push_back("delete");
    children.push_back("set");
    children.push_back("get");
    children.push_back("imaginary");
    obj->setAttr("children", children);
    atlasObjects["action"] = obj;

    obj = new Create();
    children = Element::ListType();
    children.push_back("combine");
    children.push_back("divide");
    children.push_back("communicate");
    obj->setAttr("children", children);
    atlasObjects["create"] = obj;

    obj = new Delete();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["delete"] = obj;

    obj = new Info();
    children = Element::ListType();
    children.push_back("perception");
    children.push_back("error");
    obj->setAttr("children", children);
    atlasObjects["info"] = obj;

    obj = new Set();
    children = Element::ListType();
    children.push_back("move");
    obj->setAttr("children", children);
    atlasObjects["set"] = obj;

    obj = new Get();
    children = Element::ListType();
    children.push_back("login");
    children.push_back("perceive");
    obj->setAttr("children", children);
    atlasObjects["get"] = obj;

    obj = new Perception();
    children = Element::ListType();
    children.push_back("feel");
    children.push_back("smell");
    children.push_back("sound");
    children.push_back("sight");
    obj->setAttr("children", children);
    atlasObjects["perception"] = obj;

    obj = new Error();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["error"] = obj;

    obj = new Combine();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["combine"] = obj;

    obj = new Divide();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["divide"] = obj;

    obj = new Communicate();
    children = Element::ListType();
    children.push_back("talk");
    obj->setAttr("children", children);
    atlasObjects["communicate"] = obj;

    obj = new Move();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["move"] = obj;

    obj = new Perceive();
    children = Element::ListType();
    children.push_back("look");
    children.push_back("listen");
    children.push_back("sniff");
    children.push_back("touch");
    obj->setAttr("children", children);
    atlasObjects["perceive"] = obj;

    obj = new Login();
    children = Element::ListType();
    children.push_back("logout");
    obj->setAttr("children", children);
    atlasObjects["login"] = obj;

    obj = new Logout();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["logout"] = obj;

    obj = new Sight();
    children = Element::ListType();
    children.push_back("appearance");
    children.push_back("disappearance");
    obj->setAttr("children", children);
    atlasObjects["sight"] = obj;

    obj = new Sound();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["sound"] = obj;

    obj = new Smell();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["smell"] = obj;

    obj = new Feel();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["feel"] = obj;

    obj = new Imaginary();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["imaginary"] = obj;

    obj = new Talk();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["talk"] = obj;

    obj = new Look();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["look"] = obj;

    obj = new Listen();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["listen"] = obj;

    obj = new Sniff();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["sniff"] = obj;

    obj = new Touch();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["touch"] = obj;

    obj = new Appearance();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["appearance"] = obj;

    obj = new Disappearance();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["disappearance"] = obj;

    obj = new RootEntity();
    children = Element::ListType();
    children.push_back("admin_entity");
    obj->setAttr("children", children);
    atlasObjects["root_entity"] = obj;

    obj = new AdminEntity();
    children = Element::ListType();
    children.push_back("account");
    children.push_back("game");
    obj->setAttr("children", children);
    atlasObjects["admin_entity"] = obj;

    obj = new Account();
    children = Element::ListType();
    children.push_back("player");
    children.push_back("admin");
    obj->setAttr("children", children);
    atlasObjects["account"] = obj;

    obj = new Player();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["player"] = obj;

    obj = new Admin();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["admin"] = obj;

    obj = new Game();
    children = Element::ListType();
    obj->setAttr("children", children);
    atlasObjects["game"] = obj;

    obj = new GameEntity();
    children = Element::ListType();
    // children.push_back("");
    obj->setAttr("children", children);
    atlasObjects["game_entity"] = obj;
    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.

}

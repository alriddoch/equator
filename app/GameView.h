// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef EQUATOR_APP_GAME_VIEW_H
#define EQUATOR_APP_GAME_VIEW_H

#include "Layer.h"

class Server;
class RenderableEntity;

namespace Eris {
  class Entity;
}

namespace WFMath {
  template<const int dim> class AxisBox;
  class Quaternion;
}

class GameView : public Layer {
  protected:
    Server & m_serverConnection;

    void orient(const WFMath::Quaternion &);
    void drawEntity(GlView & view, Eris::Entity *, RenderableEntity *,
                    const PosType &);
    void drawWorld(GlView & view, Eris::Entity * wrld);
  public:
    GameView(Model & m, Server & m);

    virtual void options();
    virtual void importFile();
    virtual void exportFile();
    virtual void selectInvert();
    virtual void selectAll();
    virtual void selectNone();

    virtual void draw(GlView & view);
    virtual bool animate(GlView & view);
    virtual void select(GlView & view, int x, int y);
    virtual void select(GlView & view, int x, int y, int fx, int fy);
    virtual void pushSelection();
    virtual void popSelection();
    virtual void dragStart(GlView & view, int x, int y);
    virtual void dragUpdate(GlView & view, const WFMath::Vector<3> &);
    virtual void dragEnd(GlView & view, const WFMath::Vector<3> &);
    virtual void insert(const PosType &);
    virtual void align(Alignment);

};

#endif // EQUATOR_APP_GAME_VIEW_H

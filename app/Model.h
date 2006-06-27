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

#ifndef EQUATOR_APP_MODEL_H
#define EQUATOR_APP_MODEL_H

#include "types.h"

#include <string>
#include <list>
#include <sigc++/trackable.h>
#include <sigc++/signal.h>

class Layer;
class Holo;
class MainWindow;

class Model : public sigc::trackable {
  private:
    static int modelCount;

    const int m_modelNo;
    int m_viewCount;
    std::list<Layer *> m_layers;
    Holo * m_backDropLayer;
    Layer * m_currentLayer;
    std::string m_name;
    float m_cursX, m_cursY, m_cursZ;
    float m_sizeX, m_sizeY, m_sizeZ;

  public:
    MainWindow & m_mainWindow;

    explicit Model(MainWindow & m);

    int getViewNo() {
        return m_viewCount++;
    }

    int getModelNo() {
        return m_modelNo;
    }

    const std::list<Layer *> & getLayers() const {
        return m_layers;
    }

    Holo * getBackDropLayer() const {
        return m_backDropLayer;
    }

    Layer * getCurrentLayer() const {
        return m_currentLayer;
    }

    void setCurrentLayer(Layer * l) {
        m_currentLayer = l;
    }

    const std::string & getName() const {
        return m_name;
    }

    void getCursor(float & x, float & y, float & z) const {
        x = m_cursX; y = m_cursY; z = m_cursZ;
        return;
    }

    void getSize(float & x, float & y, float & z) const {
        x = m_sizeX; y = m_sizeY; z = m_sizeZ;
        return;
    }

    void setCursor(float x, float y, float z) {
        m_cursX = x; m_cursY = y; m_cursZ = z;
        cursorMoved.emit();
        return;
    }

    void options();
    void importFile();
    void exportFile();

    void selectInvert();
    void selectAll();
    void selectNone();

    void setName(const std::string & n);
    void addLayer(Layer *);
    void update();

    void raiseCurrentLayer();
    void lowerCurrentLayer();

    void pushSelection();
    void popSelection();

    void alignSelection(Alignment );

    sigc::signal<void> nameChanged;
    sigc::signal<void> updated;
    sigc::signal<void> layersChanged;
    sigc::signal<void> typesAdded;
    sigc::signal<void> cursorMoved;
};

#endif // EQUATOR_APP_MODEL_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_MODEL_H
#define EQUATOR_APP_MODEL_H

#include <string>
#include <list>
#include <sigc++/object.h>
#include <sigc++/signal_system.h>

class Layer;
class Server;
class MainWindow;

class Model : public SigC::Object {
  private:
    static int modelCount;

    const int m_modelNo;
    int m_viewCount;
    std::list<Layer *> m_layers;
    Layer * m_currentLayer;
    Server * m_server;
    std::string m_name;

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

    Layer * getCurrentLayer() const {
        return m_currentLayer;
    }

    void setCurrentLayer(Layer * l) {
        m_currentLayer = l;
    }

    Server * getServer() const {
        return m_server;
    }

    void setServer(Server * s) {
        m_server = s;
    }

    const std::string & getName() const {
        return m_name;
    }

    void importFile();

    void setName(const std::string & n);
    void addLayer(Layer *);
    void update();

    void raiseCurrentLayer();
    void lowerCurrentLayer();

    void pushSelection();
    void popSelection();

    SigC::Signal0<void> nameChanged;
    SigC::Signal0<void> updated;
    SigC::Signal0<void> layersChanged;
    SigC::Signal0<void> typesAdded;
};

#endif // EQUATOR_APP_MODEL_H

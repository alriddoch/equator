// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "Model.h"
#include "Holo.h"

#include <iostream>

#include <cassert>

static const bool pretty = false;

int Model::modelCount = 0;

Model::Model(MainWindow & mw) : m_modelNo(modelCount++), m_viewCount(0),
                                m_cursX(0.0f), m_cursY(0.0f), m_cursZ(0.0f),
                                m_sizeX(500.0f), m_sizeY(500.0f),
                                m_sizeZ(500.0f), m_mainWindow(mw)
{
    m_currentLayer = m_backDropLayer = new Holo(*this);
    m_layers.push_front( m_currentLayer );
}

void Model::options()
{
    if (m_currentLayer != 0) {
        m_currentLayer->options();
    }
}

void Model::importFile()
{
    if (m_currentLayer != 0) {
        m_currentLayer->importFile();
    }
}

void Model::exportFile()
{
    if (m_currentLayer != 0) {
        m_currentLayer->exportFile();
    }
}

void Model::selectInvert()
{
    if (m_currentLayer != 0) {
        m_currentLayer->selectInvert();
    }
}

void Model::selectAll()
{
    if (m_currentLayer != 0) {
        m_currentLayer->selectAll();
    }
}

void Model::selectNone()
{
    if (m_currentLayer != 0) {
        m_currentLayer->selectNone();
    }
}

void Model::addLayer(Layer * layer)
{
    std::list<Layer *>::iterator I = m_layers.begin();
    for (; I != m_layers.end() && m_currentLayer != *I; I++) {}
    if (I != m_layers.end()) { ++I; }
    m_layers.insert(I, layer);
    m_currentLayer = layer;
    layersChanged.emit();
    updated.emit();
}

void Model::update()
{
    updated.emit();
}

void Model::raiseCurrentLayer()
{
    std::list<Layer *>::iterator I = m_layers.begin();
    for (; I != m_layers.end() && m_currentLayer != *I; I++) {}
    if (I == m_layers.end()) { return; }
    if (I == m_layers.begin()) { return; }
    I = m_layers.erase(I);
    m_layers.insert(--I, m_currentLayer);
    layersChanged.emit();
    updated.emit();
}

void Model::lowerCurrentLayer()
{
    std::list<Layer *>::iterator I = m_layers.begin();
    for (; I != m_layers.end() && m_currentLayer != *I; I++) {}
    if (I == m_layers.end()) { return; }
    I = m_layers.erase(I);
    if (I != m_layers.end()) { ++I; }
    m_layers.insert(I, m_currentLayer);
    layersChanged.emit();
    updated.emit();
}

void Model::pushSelection()
{
    assert(m_currentLayer != NULL);

    m_currentLayer->pushSelection();
}

void Model::popSelection()
{
    assert(m_currentLayer != NULL);

    m_currentLayer->popSelection();
}

void Model::alignSelection(Alignment e)
{
    assert(m_currentLayer != NULL);

    m_currentLayer->align(e);
}

void Model::setName(const std::string & n)
{
    m_name = n;
    nameChanged.emit();
}

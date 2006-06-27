// Equator Online RPG World Building Tool
// Copyright (C) 2000-2003 Alistair Riddoch
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

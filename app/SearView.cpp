// Equator Online RPG World Building Tool
// Copyright (C) 2006 Alistair Riddoch
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

#include "SearView.h"

#include "Model.h"

SearView::SearView(Model & m) : Layer(m, m.getName(), "Sear View")
{
}

void SearView::options()
{
}

void SearView::importFile()
{
}

void SearView::exportFile()
{
}

void SearView::selectInvert()
{
}

void SearView::selectAll()
{
}

void SearView::selectNone()
{
}


void SearView::draw(GlView & view)
{
}

bool SearView::animate(GlView & view)
{
    return false;
}

void SearView::select(GlView & view, int x, int y)
{
}

void SearView::select(GlView & view, int x, int y, int fx, int fy)
{
}

void SearView::pushSelection()
{
}

void SearView::popSelection()
{
}

void SearView::dragStart(GlView & view, int x, int y)
{
}

void SearView::dragUpdate(GlView & view, const WFMath::Vector<3> &)
{
}

void SearView::dragEnd(GlView & view, const WFMath::Vector<3> &)
{
}

void SearView::insert(const PosType &)
{
}

void SearView::align(Alignment)
{
}


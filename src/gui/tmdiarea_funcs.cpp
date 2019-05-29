/*
	*** Custom MDI Area
	*** src/gui/tmdiarea_funcs.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/tmdiarea.hui"
#include "gui/gui.h"
#include "main/dissolve.h"

// Constructor
TMdiArea::TMdiArea(DissolveWindow* dissolveWindow, QWidget* parent) : QMdiArea(parent)
{
	dissolveWindow_ = dissolveWindow;

	setFont(dissolveWindow_->font());

	// Set up the area
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setContextMenuPolicy(Qt::CustomContextMenu);
}

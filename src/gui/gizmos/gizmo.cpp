/*
	*** Gizmo
	*** src/gui/gizmos/gizmo.cpp
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

#include "gui/gizmos/gizmo.h"

// Constructor
Gizmo::Gizmo(Dissolve& dissolve) : ListItem<Gizmo>(), dissolve_(dissolve)
{
	window_ = NULL;
	refreshing_ = false;
}

Gizmo::~Gizmo()
{
}

/*
 * Core
 */

// Set title of widget
void Gizmo::setTitle(const char* title)
{
	title_ = title;
}

// Return title of widget (and associated window title, if in a window)
const char* Gizmo::title()
{
	return title_.get();
}

// Set QMdiSubWindow containing the Gizmo
void Gizmo::setWindow(QMdiSubWindow* window)
{
	window_ = window;
}

// Return SubWindow in which this widget is displayed
QMdiSubWindow* Gizmo::window()
{
	return window_;
}

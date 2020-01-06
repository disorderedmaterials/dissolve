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
Gizmo::Gizmo(Dissolve& dissolve, const char* uniqueName) : ListItem<Gizmo>(), dissolve_(dissolve)
{
	window_ = NULL;
	uniqueName_ = uniqueName;
	refreshing_ = false;
}

Gizmo::~Gizmo()
{
}

/*
 * Core
 */

// Set unique name of widget
void Gizmo::setUniqueName(const char* uniqueName)
{
	uniqueName_ = uniqueName;
}

// Return unique name of widget
const char* Gizmo::uniqueName()
{
	return uniqueName_.get();
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

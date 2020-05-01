/*
	*** Configuration Viewer - Input
	*** src/gui/configurationviewer_input.cpp
	Copyright T. Youngs 2019-2020

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

#include "classes/configuration.h"
#include "gui/configurationviewer.hui"
#include "gui/render/renderableconfiguration.h"
#include <QtGui/QMouseEvent>

// Mouse moved
void ConfigurationViewer::mouseMoved(int dx, int dy)
{
	// If we are not actually interacting with the view, return now
	if (!interacting())
		return;

	bool refresh = false;
	Atom *currentAtom = NULL;

	// What we do here depends on the current mode
	switch (interactionMode())
	{
		case (ConfigurationViewer::RotateViewInteraction):
			// Rotate view
			if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
			{
			}
			else if (mouseDownModifiers_.testFlag(Qt::ControlModifier))
			{
			}
			else
			{
				view_.rotateView(-dy / 2.0, dx / 2.0);
				refresh = true;
			}
			break;
		case (ConfigurationViewer::TranslateViewInteraction):
			// If this is a flat view, shift the axis limits rather than translating the view
			if (view_.isFlatView())
				view_.shiftFlatAxisLimits(dx, dy);
			else
				view_.translateView(dx / 15.0, dy / 15.0, 0.0);
			refresh = true;
			break;
		default:
			break;
	}

	if (refresh)
		postRedisplay();
}

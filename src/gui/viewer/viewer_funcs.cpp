/*
	*** Base Viewer Functions
	*** src/gui/viewer/viewer_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/viewer.hui"
#include "base/messenger.h"

// Constructor
BaseViewer::BaseViewer(QWidget* parent) : QOpenGLWidget(parent), view_(renderables_, fontInstance_)
{
	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffScreen_ = false;
	offscreenBuffer_ = NULL;
	pixelScaling_ = 1.0;

	// Options
	axesVisible_ = true;
	clipToAxesVolume_ = true;

	// Interaction
	interacting_ = false;
	interactionMode_ = -1;

	// Object Querying
	queryingObjects_ = false;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);

	// Set a surface format to enable multisampling
	// NOTE This used to be performed by setting the default surface format within main(), but this created severe unwanted widget behaviour
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);
	setFormat(surfaceFormat);
}

// Destructor
BaseViewer::~BaseViewer()
{
}

/*
 * Options
 */

// Set whether Axes are visible
void BaseViewer::setAxesVisible(bool visible)
{
	axesVisible_ = visible;
}

// Return whether Axes are visible
bool BaseViewer::axesVisible() const
{
	return axesVisible_;
}

// Set whether to apply clip planes about Axes volume for Renderables
void BaseViewer::setClipToAxesVolume(bool clip)
{
	clipToAxesVolume_ = clip;
}

// Return whether clip planes are applied about Axes volume for Renderables
bool BaseViewer::clipToAxesVolume() const
{
	return clipToAxesVolume_;
}

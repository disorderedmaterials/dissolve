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

	// Query
	objectQueryX_ = -1;
	objectQueryY_ = -1;
	depthAtQueryCoordinates_ = 1.0;
	objectAtQueryCoordinates_ = BaseViewer::NoObject;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);

        // Set up the font instance
	fontInstance_.setUp();
}

// Destructor
BaseViewer::~BaseViewer()
{
}

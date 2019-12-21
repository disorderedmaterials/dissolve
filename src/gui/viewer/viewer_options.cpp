/*
	*** Base Viewer - Options
	*** src/gui/viewer/viewer_options.cpp
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

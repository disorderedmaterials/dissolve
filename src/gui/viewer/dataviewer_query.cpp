/*
	*** Data Viewer - Object Querying
	*** src/gui/viewer/dataviewer_query.cpp
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

#include "gui/viewer/dataviewer.hui"
#include "base/messenger.h"

// Update depth at query coordinates, returning whether it is closer
bool DataViewer::updateQueryDepth()
{
	// Return immediately if we are not querying
	if (objectQueryX_ == -1) return false;

	// Sample a small area centred at the supplied position
	GLfloat depth[9];
	
	glReadPixels(objectQueryX_, objectQueryY_, objectQueryWidth_, objectQueryHeight_, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	bool result = false;
	for (int i=0; i<objectQueryWidth_*objectQueryHeight_; ++i)
	{
		if (depth[i] < depthAtQueryCoordinates_)
		{
			depthAtQueryCoordinates_ = depth[i];
			result = true;
		}
	}
	// 	printf("Current averageDepth at %i,%i is now %f\n", objectQueryX_, objectQueryY_, averageDepth);

	return result;
}

// Set information of query object
void DataViewer::setQueryObject(DataViewer::ViewObject objectType, const char* info)
{
	objectAtQueryCoordinates_ = objectType;
	infoAtQueryCoordinates_ = info;
}

// Set coordinates to query at next redraw
void DataViewer::setQueryCoordinates(int mouseX, int mouseY)
{
	depthAtQueryCoordinates_ = 1.0;
	objectAtQueryCoordinates_ = DataViewer::NoObject;
	infoAtQueryCoordinates_.clear();

	// Check for invalid coordinates
	if ((mouseX < 0) || (mouseX >= width()) || (mouseY < 0) || (mouseY >= height()))
	{
		objectQueryX_ = -1;
		objectQueryY_ = -1;
		return;
	}

	// Setup area to sample around central pixel
	objectQueryWidth_ = 3;
	objectQueryX_ = mouseX-1;
	if (mouseX == 0)
	{
		--objectQueryWidth_;
		++mouseX;
	}
	else if (mouseX == (width()-1)) --objectQueryWidth_;

	objectQueryHeight_ = 3;
	objectQueryY_ = mouseY-1;
	if (mouseY == 0)
	{
		--objectQueryHeight_;
		++mouseY;
	}
	else if (mouseY == (height()-1)) --objectQueryHeight_;
}

// Return object type at query coordinates
DataViewer::ViewObject DataViewer::objectAtQueryCoordinates() const
{
	return objectAtQueryCoordinates_;
}

// Info for object at query coordinates
const char* DataViewer::infoAtQueryCoordinates() const
{
	return infoAtQueryCoordinates_.get();
}

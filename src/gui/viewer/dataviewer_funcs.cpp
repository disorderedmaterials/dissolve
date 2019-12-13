/*
	*** Data Viewer - Functions
	*** src/gui/viewer/dataviewer_funcs.cpp
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

// Static Singletons
RefList<DataViewer> DataViewer::renderableDestinations_;

// Constructor
DataViewer::DataViewer(QWidget* parent) : BaseViewer(parent)
{
	// Interaction
	setInteractionMode(DataViewer::DefaultInteraction);

	// Set up the view
	view_.setViewType(View::FlatXYView);
}

// Destructor
DataViewer::~DataViewer()
{
	// Remove ourselves from the destination list if appropriate
	if (renderableDestination_) renderableDestinations_.remove(this);
}

/*
 * Destinations
 */

// Set that this DataViewer should be a Renderable destination
void DataViewer::enableAsRenderableDestination()
{
	renderableDestination_ = true;

	renderableDestinations_.addUnique(this);
}

// Set name of this renderable destination
void DataViewer::setDestinationName(const char* name)
{
	destinationName_ = name;
}

// Return name of this renderable destination
const char* DataViewer::destinationName() const
{
	return destinationName_.get();
}

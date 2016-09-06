/*
	*** Species Viewer Functions
	*** src/gui/speciesviewer_funcs.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/speciesviewer.uih"
#include "base/messenger.h"

// Constructor
SpeciesViewer::SpeciesViewer(QWidget* parent) : Viewer(parent)
{
	// Source data
	sourceSpecies_ = NULL;
	drawStyle_ = SpeciesViewer::LineStyle;

	// Input
// 	atomContextMenu_ = NULL;
// 	speciesUpdateTargets_ = 0;

	createPrimitives();
}

// Destructor
SpeciesViewer::~SpeciesViewer()
{
}


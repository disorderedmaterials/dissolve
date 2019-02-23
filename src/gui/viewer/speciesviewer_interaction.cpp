/*
	*** Species Viewer - Interaction
	*** src/gui/viewer/speciesviewer_interaction.cpp
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

#include "gui/viewer/speciesviewer.hui"

// Return current interaction mode
int SpeciesViewer::interactionMode() const
{
}

// Start interaction at the specified screen coordinates
void SpeciesViewer::startInteraction(Qt::KeyboardModifiers modifiers)
{
}

// End interaction at the specified screen coordinates
void SpeciesViewer::endInteraction()
{
}

// Cancel current interaction
void SpeciesViewer::cancelInteraction()
{
	// Perform any actions necessary to properly cancel the interaction
	switch (interactionMode_)
	{
		default:
			break;
	}

	// Reset back to DefaultInteraction
	interactionMode_ = SpeciesViewer::DefaultInteraction;
}

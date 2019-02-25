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

// Start interaction at the specified screen coordinates
void SpeciesViewer::startInteraction(Qt::KeyboardModifiers modifiers)
{
	switch (interactionMode_)
	{
		// Default Interaction Mode
		case (SpeciesViewer::DefaultInteraction):
			// This is the standard mode, giving access to view manipulation
			if (buttonState_.testFlag(Qt::LeftButton)) interactionMode_ = SpeciesViewer::SelectAreaInteraction;
			else if (buttonState_.testFlag(Qt::RightButton)) interactionMode_ = SpeciesViewer::RotateViewInteraction;
			else if (buttonState_.testFlag(Qt::MiddleButton)) interactionMode_ = SpeciesViewer::TranslateViewInteraction;
			break;
		default:
			break;
	}
}

// End interaction at the specified screen coordinates
void SpeciesViewer::endInteraction()
{
	// Finalise interaction type
	switch (interactionMode_)
	{
		case (SpeciesViewer::SelectAreaInteraction):
			// Check the pixel area of the clicked region and determine whether this was actually a targeted click rather than an area select
			if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
			{
				// Single, targetted click - atom under mouse?
				// TODO
			}
			else
			{
				// Click-drag - area select
			}
			break;
		case (SpeciesViewer::RotateViewInteraction):
			// Nothing more to do - rotation matrix has already been modified
			break;
		case (SpeciesViewer::TranslateViewInteraction):
			// Nothing more to do - translation has already been applied
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}

	// Reset (cancel) the interaction mode
	cancelInteraction(); 
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

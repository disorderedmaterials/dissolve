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
#include "classes/species.h"
#include "classes/speciesatom.h"

/*
 * Private Functions
 */

// Return atom at specified coordinates
SpeciesAtom* SpeciesViewer::atomAt(int x, int y)
{
	if (!species_) return NULL;

	double lengthScale = 0.3;
	Vec3<double> rScreen;

	ListIterator<SpeciesAtom> atomIterator(species_->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		lengthScale = 0.3;
		rScreen = view_.dataToScreen(i->r(), lengthScale);
		rScreen.x -= x;
		rScreen.y -= y;
		if (sqrt(rScreen.x*rScreen.x + rScreen.y*rScreen.y) < lengthScale) return i;
	}

	return NULL;
}

/*
 * Protected Virtuals
 */

// Start interaction at the specified screen coordinates
void SpeciesViewer::startInteraction()
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
		// Draw
		case (SpeciesViewer::DrawInteraction):
			clickedAtom_ = atomAt(rMouseDown_.x, rMouseDown_.y);
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
				SpeciesAtom* i = atomAt(rMouseLast_.x, rMouseLast_.y);
			}
			else
			{
				// Click-drag - area select
			}
			// Revert to default interaction mode
			interactionMode_ = SpeciesViewer::DefaultInteraction;
			break;
		case (SpeciesViewer::RotateViewInteraction):
			// Rotation matrix has already been modified. Revert to default interaction mode
			interactionMode_ = SpeciesViewer::DefaultInteraction;
			break;
		case (SpeciesViewer::TranslateViewInteraction):
			// Translation has already been applied. Revert to default interaction mode
			interactionMode_ = SpeciesViewer::DefaultInteraction;
			break;
		case (SpeciesViewer::DrawInteraction):
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}
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

	// Reset other data
	clickedAtom_ = NULL;

	// Reset back to DefaultInteraction
	interactionMode_ = SpeciesViewer::DefaultInteraction;
}

/*
	*** Data Viewer - Interaction
	*** src/gui/viewer/dataviewer_interaction.cpp
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

/*
 * Public Functions
 */

// Return interaction mode
DataViewer::InteractionMode DataViewer::interactionMode() const
{
	return interactionMode_;
}

// Return current axis target for interaction
int DataViewer::interactionAxis() const
{
	return interactionAxis_;
}

// Cancel current interaction
void DataViewer::cancelInteraction()
{
	// Perform any actions necessary to properly cancel the interaction
	switch (interactionMode_)
	{
		default:
			break;
	}

	// Reset back to DefaultInteraction
	interactionMode_ = DataViewer::DefaultInteraction;
}

// Start interaction at the specified screen coordinates
void DataViewer::startInteraction(Qt::KeyboardModifiers modifiers)
{
	switch (interactionMode_)
	{
		// Default Interaction Mode
		case (DataViewer::DefaultInteraction):
			// This is the standard mode, giving access to view manipulation
			if (buttonState_.testFlag(Qt::LeftButton)) interactionMode_ = DataViewer::ZoomToAreaInteraction;
			else if (buttonState_.testFlag(Qt::RightButton)) interactionMode_ = DataViewer::RotateViewInteraction;
			else if (buttonState_.testFlag(Qt::MiddleButton)) interactionMode_ = DataViewer::TranslateViewInteraction;
			break;
		default:
			break;
	}

	// Calculate axis value at start of interaction REMOVE
// 	if (interactionAxis_ != -1) clickedInteractionValue_ = view_.screenToAxis(interactionAxis_, rMouseDown_.x, rMouseDown_.y, true);
}

// Update current interaction position / coordinate, returning if a refresh of the display is necessary
bool DataViewer::updateInteractionPosition(int mouseX, int mouseY)
{
	bool refresh = false;

	// Are we interacting with an axis?
	if (interactionAxis_ != -1)
	{
		// Calculate axis value
		currentInteractionValue_ = view_.screenToAxis(interactionAxis_, mouseX, mouseY, true);

		refresh = true;
	}
	else
	{
		clickedInteractionValue_ = 0.0;
		currentInteractionValue_ = 0.0;
	}

	return refresh;
}

// End interaction at the specified screen coordinates
void DataViewer::endInteraction()
{
	ViewerObject clickedObject = NoObject;
	CharString clickedObjectInfo;
	double newMin, newMax;

	// Finalise interaction type
	switch (interactionMode_)
	{
		case (DataViewer::ZoomToAreaInteraction):
			// Check the pixel area of the clicked region and determine whether this was actually a targeted click rather than an area select
			if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
			{
				// Single, targetted click - get the clicked object
				setQueryCoordinates(rMouseLast_.x, rMouseLast_.y);
				// TODO Offscreen paint instead of repaint
				repaint();
				clickedObject = objectAtQueryCoordinates();
				clickedObjectInfo = infoAtQueryCoordinates();
			}
			else
			{
				// Click-drag
				if (view_.isFlatView())
				{
					// Make sure any autofollowing is turned off...
					view_.setAutoFollowType(View::NoAutoFollow);

					// Zoom to the specified coordinate range
					view_.zoomTo(clicked2DAxesCoordinates(), current2DAxesCoordinates());
				}
				else
				{
				}
			}
			break;
		case (DataViewer::RotateViewInteraction):
			// Nothing more to do - rotation matrix has already been modified
			break;
		case (DataViewer::TranslateViewInteraction):
			// Nothing more to do - translation has already been applied
			break;
// 		case (DataViewer::ZoomInteraction):
// 			// None : Zoom to defined region
// 			newMin = std::min(clickedInteractionValue_, currentInteractionValue_);
// 			newMax = std::max(clickedInteractionValue_, currentInteractionValue_);
// 			if ((newMax-newMin) > 1.0e-10)
// 			{
// 				view_.axes().setMin(interactionAxis_, newMin);
// 				view_.axes().setMax(interactionAxis_, newMax);
// // 				axesWindow_.updateControls();
// 			}
// 			postRedisplay();
// 			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}

	// Reset (cancel) the interaction mode
	cancelInteraction(); 
}

// Return clicked interaction value on axis
double DataViewer::clickedInteractionValue()
{
	return clickedInteractionValue_;
}

// Return current interaction value on axis
double DataViewer::currentInteractionValue()
{
	return currentInteractionValue_;
}

// Return clicked interaction coordinate on axis
double DataViewer::clickedInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	Axes& axes = view_.axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
}

// Return current interaction coordinate on axis
double DataViewer::currentInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	Axes& axes = view_.axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
}

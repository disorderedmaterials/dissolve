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
}

// End interaction at the specified screen coordinates
void DataViewer::endInteraction()
{
	ViewerObject clickedObject = NoObject;
	CharString clickedObjectInfo;
	double rangeStart, rangeEnd;

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
		case (DataViewer::ZoomXRangeInteraction):
			// Zoom to defined X range
			rangeStart = view_.screenToAxis(0, rMouseDown_.x, rMouseDown_.y, true);
			rangeEnd = view_.screenToAxis(0, rMouseLast_.x, rMouseLast_.y, true);
			if (fabs(rangeStart-rangeEnd) > 1.0e-10)
			{
				view_.axes().setMin(0, std::min(rangeStart, rangeEnd));
				view_.axes().setMax(0, std::max(rangeStart, rangeEnd));
			}
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}

	// Reset (cancel) the interaction mode
	cancelInteraction(); 
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

// // Return clicked interaction coordinate on axis
// double DataViewer::clickedInteractionCoordinate()
// {
// 	// Check for valid interaction axis
// 	if (interactionAxis_ == -1) return 0.0;
// 
// 	Axes& axes = view_.axes();
// 	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_));
// 	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
// }
// 
// // Return current interaction coordinate on axis
// double DataViewer::currentInteractionCoordinate()
// {
// 	// Check for valid interaction axis
// 	if (interactionAxis_ == -1) return 0.0;
// 
// 	Axes& axes = view_.axes();
// 	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_));
// 	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
// }

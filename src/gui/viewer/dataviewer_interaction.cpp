/*
	*** Data Viewer - Interaction
	*** src/gui/viewer/dataviewer_interaction.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer/dataviewer.hui"

// Static list of interaction modes
const char* InteractionModeName[] = { 
	"View",
	"Zoom"
};

const char* InteractionModeHelp[] = {
	"<b>Right</b> Rotate view, <b>Middle</b> Translate view (3D) or shift axis limits (2D), <b>Wheel</b> Zoom view",
	"<b>Left-click-drag</b> Zoom to selected region, <b>Ctrl+Left</b> Extract slice"
};

// Return InteractionMode name
const char* DataViewer::interactionModeName(DataViewer::InteractionMode mode)
{
	return InteractionModeName[mode];
}

// Return InteractionMode help text
const char* DataViewer::interactionModeHelp(DataViewer::InteractionMode mode)
{
	return InteractionModeHelp[mode];
}

/*
 * Public Functions
 */

// Set interaction mode and axis
void DataViewer::setInteractionMode(DataViewer::InteractionMode mode, int axis)
{
	// Cancel any previous interaction mode...
	cancelInteraction();

	interactionMode_ = mode;
	interactionAxis_ = axis;

	// Generate interaction primitives for all panes
	view_.updateInteractionPrimitive(axis);

	// Update GUI
	postRedisplay();
}

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

// Return whether interaction has started (i.e. mouse is down)
bool DataViewer::interactionStarted() const
{
	return interactionStarted_;
}

// Cancel current interaction
void DataViewer::cancelInteraction()
{
	// Cancel interaction type
	switch (interactionMode_)
	{
		case (DataViewer::ViewInteraction):
		case (DataViewer::ZoomInteraction):
			break;
		default:
			printf("Internal Error: Don't know how to cancel interaction mode %i\n", interactionMode_);
			break;
	}
}

// Start interaction at the specified screen coordinates
void DataViewer::startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers)
{
	// If a 2D view, store the clicked local coordinate
	if (view_.isFlatView()) rClickedLocal_ = screenToLocal(mouseX, mouseY);

	// Calculate axis value at start of interaction
	if (interactionAxis_ != -1) clickedInteractionValue_ = view_.screenToAxis(interactionAxis_, mouseX, mouseY, true);

	// Store keyboard modifiers
	clickedInteractionModifiers_ = modifiers;

	interactionStarted_ = true;
}

// Update current interaction position / coordinate, returning if a refresh of the display is necessary
bool DataViewer::updateInteractionPosition(int mouseX, int mouseY)
{
	bool refresh = false;

	// If a 2D view, store the current local coordinate
	if (view_.isFlatView()) rCurrentLocal_ = screenToLocal(mouseX, mouseY);

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

	// Update the displayed coordinate box informstion
// 	updateCoordinateInfo();

	return refresh;
}

// End interaction at the specified screen coordinates
void DataViewer::endInteraction(int mouseX, int mouseY)
{
	ViewObject clickedObject = NoObject;
	CharString clickedObjectInfo;
	double newMin, newMax;

	// Finalise interaction type
	switch (interactionMode_)
	{
		case (DataViewer::ViewInteraction):
			// Check the pixel area of the clicked region and determine whether this was a targeted click or a click-drag
			if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
			{
				// Get the clicked object (if interaction mode is ViewInteraction)
				setQueryCoordinates(mouseX, mouseY);
				repaint();
				clickedObject = objectAtQueryCoordinates();
				clickedObjectInfo = infoAtQueryCoordinates();
				if (clickedObject == CollectionObject) highlightedRenderable_ = renderable(clickedObjectInfo);
			}
			else
			{
				// Click-drag
				// Single, targetted click
				if (view_.isFlatView())
				{
					// Make sure any autofollowing is turned off...
					view_.setAutoFollowType(View::NoAutoFollow);

					// Zoom to the specified coordinate range
					view_.zoomTo(rClickedLocal_, rCurrentLocal_);
				}
				else
				{
				}
			}

			if (clickedObject == CollectionObject)
			{
				highlightedRenderable_ = NULL;
				Renderable* rend = renderable(clickedObjectInfo);
				if (rend) setCurrentRenderable(rend);
			}
			break;
		case (DataViewer::ZoomInteraction):
			// None : Zoom to defined region
			newMin = std::min(clickedInteractionValue_, currentInteractionValue_);
			newMax = std::max(clickedInteractionValue_, currentInteractionValue_);
			if ((newMax-newMin) > 1.0e-10)
			{
				view_.axes().setMin(interactionAxis_, newMin);
				view_.axes().setMax(interactionAxis_, newMax);
// 				axesWindow_.updateControls();
			}
			postRedisplay();
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}

	interactionStarted_ = false;
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

// Perform relevant double-click action, occurring at specified coordinate
void DataViewer::doubleClickInteraction(int mouseX, int mouseY)
{
	setQueryCoordinates(mouseX, mouseY);
	repaint();

	switch (objectAtQueryCoordinates())
	{
		case (NoObject):
			break;
		case (AxisLineObject):
//			i = clickedObjectInfo_.asInteger();
// 			axesWindow_.updateAndShow();
// 			axesWindow_.ui.AxesTabs->setCurrentIndex(i);
// 			axesWindow_.ui.
			break;
		case (AxisTickLabelObject):
			break;
		case (AxisTitleLabelObject):
			break;
		case (CollectionObject):
			break;
		case (GridLineMajorObject):
			break;
		case (GridLineMinorObject):
			break;
	}

	// Reset clicked object info
// 	clickedObject_ = NoObject;
// 	clickedObjectInfo_.clear();
}

// Calculate local coordinate from supplied screen coordinates
Vec3<double> DataViewer::screenToLocal(int mouseX, int mouseY)
{
	// Must be a flat view in order to return 2D local coordinates
	if (!view_.isFlatView()) return Vec3<double>();

	Vec3<double> result;
	if (view_.viewType() == View::FlatXYView) result.set(view_.screenToAxis(0, mouseX, mouseY, false), view_.screenToAxis(1, mouseX, mouseY, false), 0.0);
	else if (view_.viewType() == View::FlatXZView) result.set(view_.screenToAxis(0, mouseX, mouseY, false), 0.0, view_.screenToAxis(2, mouseX, mouseY, false));
	else if (view_.viewType() == View::FlatZYView) result.set(0.0, view_.screenToAxis(1, mouseX, mouseY, false), view_.screenToAxis(2, mouseX, mouseY, false));

	return result;
}

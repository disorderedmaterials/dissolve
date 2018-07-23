/*
	*** UChroma - Interaction
	*** src/gui/uchroma_interaction.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/gui/operate_bgsub.h"

// Static list of interaction modes
const char* InteractionModeName[] = { 
	"Fit (select X)",
	"Fit (select Z)",
	"BG Subtraction (select Y)",	 
	"BG Subtraction (select X range)", 
	"BG Subtraction (select Z range)", 
	"View",
	"Zoom"
};

const char* InteractionModeHelp[] = {
	"<b>Left-click-drag</b> Select X region for fitting",
	"<b>Left-click-drag</b> Select Z region for fitting",
	"<b>Left-click</b> select Y value for subtraction",
	"<b>Left-click-drag</b> select X range for determination of average value",
	"<b>Left-click-drag</b> select Z range for determination of average value",
	"<b>Right</b> Rotate view, <b>Middle</b> Translate view (3D) or shift axis limits (2D), <b>Wheel</b> Zoom view",
	"<b>Left-click-drag</b> Zoom to selected region, <b>Ctrl+Left</b> Extract slice"
};

// Return InteractionMode name
const char* UChromaBase::interactionModeName(UChromaBase::InteractionMode mode)
{
	return InteractionModeName[mode];
}

// Return InteractionMode help text
const char* UChromaBase::interactionModeHelp(UChromaBase::InteractionMode mode)
{
	return InteractionModeHelp[mode];
}

/*
 * Public Functions
 */

// Set interaction mode and axis
void UChromaBase::setInteractionMode(UChromaBase::InteractionMode mode, int axis)
{
	// Cancel any previous interaction mode...
	cancelInteraction();

	interactionMode_ = mode;
	interactionAxis_ = axis;

	// Generate interaction primitives for all panes
	viewLayout().updateInteractionPrimitives(axis);

	// Update GUI
	updateGUI();
}

// Return interaction mode
UChromaBase::InteractionMode UChromaBase::interactionMode() const
{
	return interactionMode_;
}

// Return current axis target for interaction
int UChromaBase::interactionAxis() const
{
	return interactionAxis_;
}

// Return whether interaction has started (i.e. mouse is down)
bool UChromaBase::interactionStarted() const
{
	return interactionStarted_;
}

// Cancel current interaction
void UChromaBase::cancelInteraction()
{
	// Cancel interaction type
	switch (interactionMode_)
	{
		case (UChromaBase::FitSetupSelectXInteraction):
		case (UChromaBase::FitSetupSelectZInteraction):
			editFitKernelDialog_.updateAndExec();
			break;
		case (UChromaBase::OperateBGSubSelectYValueInteraction):
		case (UChromaBase::OperateBGSubSelectXRangeInteraction):
		case (UChromaBase::OperateBGSubSelectZRangeInteraction):
			OperateBGSubDialog(*this, parent_).updateAndExec();
			break;
		case (UChromaBase::ViewInteraction):
		case (UChromaBase::ZoomInteraction):
			break;
		default:
			printf("Internal Error: Don't know how to cancel interaction mode %i\n", interactionMode_);
			break;
	}
}

// Start interaction at the specified screen coordinates
void UChromaBase::startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers)
{
	// Check interaction pane
	if (!ViewPane::objectValid(currentViewPane_))
	{
		Messenger::print("Internal Error: Invalid interaction pane - refusing to start interaction.\n");
		return;
	}

	rClicked_.set(mouseX, mouseY, 0.0);

	// If a 2D view, store the clicked coordinate
	if (currentViewPane_->isFlatView()) rClickedLocal_ = screenToLocal(mouseX, mouseY);

	// Calculate axis value at start of interaction
	if (interactionAxis_ != -1) clickedInteractionValue_ = currentViewPane_->screenToAxis(interactionAxis_, mouseX, mouseY, true);

	// Store keyboard modifiers
	clickedInteractionModifiers_ = modifiers;

	interactionStarted_ = true;
}

// Update current interaction position / coordinate, returning if a refresh of the display is necessary
bool UChromaBase::updateInteractionPosition(int mouseX, int mouseY)
{
	if (!currentViewPane_) return false;

	bool refresh = false;

	// If a 2D view, store the coordinate
	if (currentViewPane_->isFlatView()) rCurrentLocal_ = screenToLocal(mouseX, mouseY);

	// Are we interacting with an axis?
	if (interactionAxis_ != -1)
	{
		// Calculate axis value
		currentInteractionValue_ = currentViewPane_->screenToAxis(interactionAxis_, mouseX, mouseY, true);

		// Extract slice from collections in current pane (unless this is a SliceMonitor)
		if (currentViewPane_->role() != ViewPane::SliceMonitorRole) currentViewPane_->collectionsUpdateCurrentSlices(interactionAxis_, currentInteractionValue_);

		refresh = true;
	}
	else
	{
		clickedInteractionValue_ = 0.0;
		currentInteractionValue_ = 0.0;
	}

	// Update the displayed coordinate box informstion
	updateCoordinateInfo();

	return refresh;
}

// End interaction at the specified screen coordinates
void UChromaBase::endInteraction(int mouseX, int mouseY)
{
	// Check interaction pane
	if (!ViewPane::objectValid(currentViewPane_))
	{
		Messenger::print("Internal Error: Invalid interaction pane - refusing to end interaction.\n");
		return;
	}

	// Finalise interaction type
	switch (interactionMode_)
	{
		case (UChromaBase::FitSetupSelectXInteraction):
			editFitKernelDialog_.ui.XAbsoluteMinSpin->setValue(std::min(clickedInteractionValue_, currentInteractionValue_));
			editFitKernelDialog_.ui.XAbsoluteMaxSpin->setValue(std::max(clickedInteractionValue_, currentInteractionValue_));
			editFitKernelDialog_.updateAndExec();
			break;
		case (UChromaBase::FitSetupSelectZInteraction):
			editFitKernelDialog_.ui.ZAbsoluteMinSpin->setValue(std::min(clickedInteractionValue_, currentInteractionValue_));
			editFitKernelDialog_.ui.ZAbsoluteMaxSpin->setValue(std::max(clickedInteractionValue_, currentInteractionValue_));
			editFitKernelDialog_.updateAndExec();
			break;
		case (UChromaBase::OperateBGSubSelectYValueInteraction):
			OperateBGSubDialog::setConstantValue(currentInteractionValue_);
			OperateBGSubDialog(*this, parent_).updateAndExec();
			break;
		case (UChromaBase::OperateBGSubSelectXRangeInteraction):
			OperateBGSubDialog::setXRange(std::min(clickedInteractionValue_, currentInteractionValue_), std::max(clickedInteractionValue_, currentInteractionValue_));
			OperateBGSubDialog(*this, parent_).updateAndExec();
			break;
		case (UChromaBase::OperateBGSubSelectZRangeInteraction):
			OperateBGSubDialog::setZRange(std::min(clickedInteractionValue_, currentInteractionValue_), std::max(clickedInteractionValue_, currentInteractionValue_));
			OperateBGSubDialog(*this, parent_).updateAndExec();
			break;
		case (UChromaBase::ViewInteraction):
			// Check the pixel area of the clicked region and determine whether this was a targeted click or a click-drag
			if ((viewer_->rMouseDown() - viewer_->rMouseLast()).magnitude() < 9.0)
			{
				// Get the clicked object (if interaction mode is ViewInteraction)
				viewer_->setQueryCoordinates(mouseX, mouseY);
				viewer_->repaint();
				clickedObject_ = viewer_->objectAtQueryCoordinates();
				clickedObjectInfo_ = viewer_->infoAtQueryCoordinates();
				if (clickedObject_ == UChromaViewer::CollectionObject)
				{
					viewer_->setHighlightCollection(locateCollection(clickedObjectInfo_));
				}
			}
			else
			{
				// Click-drag
				// Single, targetted click
				if (currentViewPane_->isFlatView())
				{
					// Make sure any autofollowing is turned off...
					currentViewPane_->setAutoFollowType(ViewPane::NoAutoFollow);

					// Zoom to the specified coordinate range
					currentViewPane_->zoomTo(rClickedLocal_, rCurrentLocal_);
				}
				else
				{
				}
			}

			if (clickedObject_ == UChromaViewer::CollectionObject)
			{
				viewer_->setHighlightCollection(NULL);
				Collection* collection = locateCollection(clickedObjectInfo_);
				if (collection) setCurrentCollection(collection);
			}
			break;
		case (UChromaBase::ZoomInteraction):
			// None : Zoom to defined region
			// Ctrl : Extract slice
			if (clickedInteractionModifiers_.testFlag(Qt::ControlModifier))
			{
				beginEditStateGroup("extract slice (%c = %e)", char(88+interactionAxis_), currentInteractionValue_);
				currentCollection()->extractCurrentSlice(interactionAxis_, currentInteractionValue_);
				endEditStateGroup();
				updateGUI();
			}
			else
			{
				double newMin = std::min(clickedInteractionValue_, currentInteractionValue_);
				double newMax = std::max(clickedInteractionValue_, currentInteractionValue_);
				if ((newMax-newMin) > 1.0e-10)
				{
					currentViewPane_->axes().setMin(interactionAxis_, newMin);
					currentViewPane_->axes().setMax(interactionAxis_, newMax);
					axesWindow_.updateControls();
				}
			}
			updateDisplay();
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}

	interactionStarted_ = false;
	clickedObject_ = UChromaViewer::NoObject;
	clickedObjectInfo_.clear();
}

// Return clicked interaction value on axis
double UChromaBase::clickedInteractionValue()
{
	return clickedInteractionValue_;
}

// Return current interaction value on axis
double UChromaBase::currentInteractionValue()
{
	return currentInteractionValue_;
}

// Return clicked interaction coordinate on axis
double UChromaBase::clickedInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	// Check for valid view pane
	if (currentViewPane_ == NULL) return 0.0;

	Axes& axes = currentViewPane_->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
}

// Return current interaction coordinate on axis
double UChromaBase::currentInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	// Check for valid view pane
	if (currentViewPane_ == NULL) return 0.0;

	Axes& axes = currentViewPane_->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
}

// Perform relevant double-click action, occurring at specified coordinate
void UChromaBase::doubleClickInteraction(int mouseX, int mouseY)
{
	viewer_->setQueryCoordinates(mouseX, mouseY);
	viewer_->repaint();
	clickedObject_ = viewer_->objectAtQueryCoordinates();
	clickedObjectInfo_ = viewer_->infoAtQueryCoordinates();

//	int i;
	switch (clickedObject_)
	{
		case (UChromaViewer::NoObject):
			break;
		case (UChromaViewer::AxisLineObject):
//			i = clickedObjectInfo_.asInteger();
// 			axesWindow_.updateAndShow();
// 			axesWindow_.ui.AxesTabs->setCurrentIndex(i);
// 			axesWindow_.ui.
			break;
		case (UChromaViewer::AxisTickLabelObject):
			break;
		case (UChromaViewer::AxisTitleLabelObject):
			break;
		case (UChromaViewer::CollectionObject):
			break;
		case (UChromaViewer::GridLineMajorObject):
			break;
		case (UChromaViewer::GridLineMinorObject):
			break;
	}

	// Reset clicked object info
	clickedObject_ = UChromaViewer::NoObject;
	clickedObjectInfo_.clear();
}

// Calculate local coordinate from supplied screen coordinates
Vec3<double> UChromaBase::screenToLocal(int mouseX, int mouseY)
{
	// Get ViewPane under mouse
	ViewPane* viewPane = viewLayout_.paneAt(mouseX, mouseY);
	if ((!viewPane) || (!viewPane->isFlatView())) return Vec3<double>();

	Vec3<double> result;
	if (viewPane->viewType() == ViewPane::FlatXYView) result.set(viewPane->screenToAxis(0, mouseX, mouseY, false), viewPane->screenToAxis(1, mouseX, mouseY, false), 0.0);
	else if (viewPane->viewType() == ViewPane::FlatXZView) result.set(viewPane->screenToAxis(0, mouseX, mouseY, false), 0.0, viewPane->screenToAxis(2, mouseX, mouseY, false));
	else if (viewPane->viewType() == ViewPane::FlatZYView) result.set(0.0, viewPane->screenToAxis(1, mouseX, mouseY, false), viewPane->screenToAxis(2, mouseX, mouseY, false));

	return result;
}

/*
    *** Data Viewer - Interaction
    *** src/gui/dataviewer_interaction.cpp
    Copyright T. Youngs 2013-2020

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

#include "gui/dataviewer.hui"

/*
 * Public Functions
 */

// Start interaction at the specified screen coordinates
void DataViewer::startInteraction()
{
    switch (interactionMode())
    {
        // Default Interaction Mode
        case (DataViewer::DefaultInteraction):
            // This is the standard mode, giving access to view manipulation
            if (buttonState_.testFlag(Qt::LeftButton))
                setInteractionMode(DataViewer::ZoomToAreaInteraction);
            else if (buttonState_.testFlag(Qt::RightButton))
                setInteractionMode(DataViewer::RotateViewInteraction);
            else if (buttonState_.testFlag(Qt::MiddleButton))
                setInteractionMode(DataViewer::TranslateViewInteraction);
            break;
        default:
            break;
    }
}

// End interaction at the specified screen coordinates
void DataViewer::endInteraction()
{
    double rangeStart, rangeEnd;

    // Finalise interaction type
    switch (interactionMode())
    {
        case (DataViewer::DefaultInteraction):
            break;
        case (DataViewer::ZoomToAreaInteraction):
            // Check the pixel area of the clicked region and determine whether this was actually a targeted click
            // rather than an area select
            if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
            {
                // Single, targetted click - get the clicked object
                // TODO What action are we performing here? Would this be better as a right-click action,
                // raising a context menu?
                // ViewerObject obj = queryAt(rMouseLast_.x, rMouseLast_.y);
                // Messenger::print("Object Type = {}, info = [{}]\n", BaseViewer::viewerObject(obj), queryObjectInfo());
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

            // Revert to default interaction mode
            setInteractionMode(DataViewer::DefaultInteraction);
            break;
        case (DataViewer::RotateViewInteraction):
            // Nothing more to do - rotation matrix has already been modified, so revert to default interaction mode
            setInteractionMode(DataViewer::DefaultInteraction);
            break;
        case (DataViewer::TranslateViewInteraction):
            // Nothing more to do - translation has already been applied, so revert to default interaction mode
            setInteractionMode(DataViewer::DefaultInteraction);
            break;
        case (DataViewer::ZoomXRangeInteraction):
            // Zoom to defined X range
            rangeStart = view_.screenToAxis(0, rMouseDown_.x, rMouseDown_.y, true);
            rangeEnd = view_.screenToAxis(0, rMouseLast_.x, rMouseLast_.y, true);
            if (fabs(rangeStart - rangeEnd) > 1.0e-10)
            {
                view_.axes().setMin(0, std::min(rangeStart, rangeEnd));
                view_.axes().setMax(0, std::max(rangeStart, rangeEnd));
            }
            break;
        default:
            Messenger::error("Internal Error: Don't know how to complete interaction mode {}\n", interactionMode());
            break;
    }
}

// Cancel current interaction
void DataViewer::cancelInteraction()
{
    // Perform any actions necessary to properly cancel the current interaction
    switch (interactionMode())
    {
        default:
            break;
    }
}

/*
 * Public Functions
 */

// Return text describing current interaction mode
const QString DataViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (DataViewer::DefaultInteraction):
            if (constView().isFlatView())
                return "2D View: <b>Left</b> Zoom to area; <b>Middle</b> Translate view";
            else
                return "3D View: <b>Right</b> Rotate view; <b>Middle</b> Translate view; <b>Wheel</b> Zoom "
                       "in/out";
        case (DataViewer::ZoomToAreaInteraction):
            return "Zoom to area";
        case (DataViewer::RotateViewInteraction):
            return "Rotate view";
        case (DataViewer::TranslateViewInteraction):
            return "Translate view";
        default:
            return "Unknown DataViewerInteraction";
    }
}

// // Return clicked interaction coordinate on axis
// double DataViewer::clickedInteractionCoordinate()
// {
// 	// Check for valid interaction axis
// 	if (interactionAxis_ == -1) return 0.0;
//
// 	Axes& axes = view_.axes();
// 	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ?
// log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_)); 	else return
// (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
// }
//
// // Return current interaction coordinate on axis
// double DataViewer::currentInteractionCoordinate()
// {
// 	// Check for valid interaction axis
// 	if (interactionAxis_ == -1) return 0.0;
//
// 	Axes& axes = view_.axes();
// 	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ?
// log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_)); 	else return
// (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
// }

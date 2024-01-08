// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataViewer.hui"

/*
 * Public Functions
 */

// Start interaction at the specified screen coordinates
void DataViewer::startInteraction()
{
    switch (interactionMode())
    {
        // Default Interaction Mode
        case (DataViewer::InteractionMode::Default):
            // This is the standard mode, giving access to view manipulation
            if (buttonState_.testFlag(Qt::LeftButton))
                setInteractionMode(DataViewer::InteractionMode::ZoomToArea);
            else if (buttonState_.testFlag(Qt::RightButton))
                transientInteractionMode_ = DataViewer::TransientInteractionMode::RotateView;
            else if (buttonState_.testFlag(Qt::MiddleButton))
                transientInteractionMode_ = DataViewer::TransientInteractionMode::TranslateView;
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
        case (DataViewer::InteractionMode::Default):
            break;
        case (DataViewer::InteractionMode::ZoomToArea):
            // Check the pixel area of the clicked region and determine whether this was actually a targeted click
            // rather than an area select
            if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
            {
                // Single, targetted click - get the clicked object
                // TODO What action are we performing here? Would this be better as a right-click action,
                // raising a context menu?
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
            setInteractionMode(DataViewer::InteractionMode::Default);
            break;
        case (DataViewer::InteractionMode::ZoomXRange):
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
            Messenger::error("Internal Error: Don't know how to complete interaction mode {}\n", (int)interactionMode());
            break;
    }

    // Reset any transient interaction
    transientInteractionMode_ = TransientInteractionMode::None;
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

// Set current interaction mode
void DataViewer::setInteractionMode(DataViewer::InteractionMode mode)
{
    // Cancel any current interaction
    cancelInteraction();

    interactionMode_ = mode;

    Q_EMIT(interactionModeChanged());
}

// Return current interaction mode
DataViewer::InteractionMode DataViewer::interactionMode() const { return interactionMode_; }

// Return text describing current interaction mode
const QString DataViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (DataViewer::InteractionMode::Default):
            if (view().isFlatView())
                return "2D View: <b>Left</b> Zoom to area; <b>Middle</b> Translate view";
            else
                return "3D View: <b>Right</b> Rotate view; <b>Middle</b> Translate view; <b>Wheel</b> Zoom "
                       "in/out";
        case (DataViewer::InteractionMode::ZoomToArea):
            return "Zoom to area";
        default:
            return "Unknown DataViewerInteraction";
    }
}

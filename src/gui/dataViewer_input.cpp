// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataViewer.hui"
#include <QMenu>
#include <QMouseEvent>

// Mouse Moved
void DataViewer::mouseMoved(int dx, int dy)
{
    // If we are not actually interacting with the view, return now
    if (!interacting())
    {
        Q_EMIT(currentCoordinateChanged());

        return;
    }

    auto refresh = false;

    // What we do here depends on the current mode
    switch (transientInteractionMode_)
    {
        // Perform primary interaction
        case (TransientInteractionMode::None):
            switch (interactionMode())
            {
                case (DataViewer::InteractionMode::ZoomToArea):
                    // No action to take - the selection box will be drawn from the clicked and current positions (already
                    // stored)
                    refresh = true;
                    break;
                case (DataViewer::InteractionMode::ZoomXRange):
                    // No action to take - the range will be drawn from the clicked and current positions (already stored)
                    refresh = true;
                    break;
                default:
                    break;
            }
            break;
        case (DataViewer::TransientInteractionMode::RotateView):
            if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
            {
            }
            else if (mouseDownModifiers_.testFlag(Qt::ControlModifier))
            {
            }
            else
            {
                view().rotateView(-dy / 2.0, dx / 2.0);
                refresh = true;
            }
            break;
        case (DataViewer::TransientInteractionMode::TranslateView):
            // Turn off autofollow if it is currently on...
            if (view().autoFollowType() != View::NoAutoFollow)
            {
                view().setAutoFollowType(View::NoAutoFollow);
                Q_EMIT(controlAspectChanged());
            }

            // If this is a flat view, shift the axis limits rather than translating the view
            if (view().isFlatView())
                view().shiftFlatAxisLimits(dx, dy);
            else
                view().translateView(dx / 15.0, dy / 15.0, 0.0);
            refresh = true;
            break;
        default:
            break;
    }

    // Notify any interested widgets that our current coordinate has changed
    Q_EMIT(currentCoordinateChanged());

    if (refresh)
        postRedisplay();
}

// Mouse 'wheeled'
void DataViewer::mouseWheeled(int delta)
{
    // Turn off autofollow if it is currently on...
    if (view().autoFollowType() != View::NoAutoFollow)
    {
        view().setAutoFollowType(View::NoAutoFollow);
        Q_EMIT(controlAspectChanged());
    }

    auto scrollUp = delta > 0;

    // Perform camera zoom in a 3D view, or view scaling in a 2D view
    if (view().isFlatView())
    {
        // Scale the range
        view().scaleRange(scrollUp ? 0.9 : 1.1);

        // Move the centre of the axes towards the current mouse position
        view().centre2DAt(current2DAxesCoordinates(), 0.1);
    }
    else
    {
        // Call the base class function
        BaseViewer::mouseWheeled(delta);

        // Display will have already been updated, so return now
        return;
    }

    postRedisplay();
}

// Mouse double clicked
void DataViewer::mouseDoubleClicked()
{
    ViewerObject objectType = queryAt(rMouseLast_.x, rMouseLast_.y);

    switch (objectType)
    {
        case (NoObject):
            break;
        case (AxisLineObject):
            break;
        case (AxisTickLabelObject):
            break;
        case (AxisTitleLabelObject):
            break;
        case (RenderableObject):
            break;
        case (GridLineMajorObject):
            break;
        case (GridLineMinorObject):
            break;
        case (CustomObject):
            break;
    }
}

// Context menu requested
void DataViewer::contextMenuRequested(QPoint pos)
{
    // Check for object under current coordinates...
    ViewerObject objectType = queryAt(rMouseLast_.x, rMouseLast_.y);

    // Set up the menu according to the clicked object
    if (objectType == BaseViewer::RenderableObject)
    {
        // Get Renderable pointer....
        auto rend = renderableWithTag(queryObjectInfo());
        if (!rend)
        {
            fmt::print("Couldn't locate renderable with tag '{}' in the DataViewer...", queryObjectInfo());
            return;
        }

        showRenderableContextMenu(pos, rend);
    }
    else
        showGeneralContextMenu(pos);
}

// Key pressed
bool DataViewer::keyPressed(int key)
{
    auto refresh = true;
    auto accept = true;
    switch (key)
    {
        case (Qt::Key_Left):
            if (view().isFlatView())
                view().shiftFlatAxisLimitsFractional(-0.1, 0.0);
            else
                view().rotateView(0.0, mouseDownModifiers_.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0);
            break;
        case (Qt::Key_Right):
            if (view().isFlatView())
                view().shiftFlatAxisLimitsFractional(0.1, 0.0);
            else
                view().rotateView(0.0, mouseDownModifiers_.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0);
            break;
        case (Qt::Key_Up):
            if (view().isFlatView())
                view().shiftFlatAxisLimitsFractional(0.0, -0.1);
            else
                view().rotateView(mouseDownModifiers_.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0, 0.0);
            break;
        case (Qt::Key_Down):
            if (view().isFlatView())
                view().shiftFlatAxisLimitsFractional(0.0, 0.1);
            else
                view().rotateView(mouseDownModifiers_.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0, 0.0);
            break;
        case (Qt::Key_A):
            // Turn off autofollow if it is currently on...
            if (view().autoFollowType() != View::NoAutoFollow)
            {
                view().setAutoFollowType(View::NoAutoFollow);
                Q_EMIT(controlAspectChanged());
            }

            if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
            {
                // Show only top 20% of vertical axis
                if (view().viewType() == View::FlatXYView)
                    view().showAllData(1.0, 0.2);
                else if (view().viewType() == View::FlatXZView)
                    view().showAllData(1.0, 1.0, 0.2);
                else if (view().viewType() == View::FlatZYView)
                    view().showAllData(1.0, 0.2);
            }
            else
                view().showAllData();
            break;
        case (Qt::Key_F):
            view().cycleAutoFollowType();
            Q_EMIT(controlAspectChanged());
            break;
        case (Qt::Key_L):
            if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
                view().axes().toggleLogarithmic(view().viewType() == View::FlatXZView ? 2 : 1);
            else
                view().axes().toggleLogarithmic(view().viewType() == View::FlatZYView ? 2 : 0);
            break;
        case (Qt::Key_S):
            groupManager_.cycleVerticalShiftAmount();
            break;
        default:
            refresh = false;
            accept = false;
            break;
    }

    // Update display if necessary
    if (refresh)
        postRedisplay();

    return accept;
}

// Key released
bool DataViewer::keyReleased(int key)
{
    auto refresh = false, accept = true;

    switch (key)
    {
        // Cycle render styles
        case (Qt::Key_F8):
            break;
        // Cycle colouring styles
        case (Qt::Key_F9):
            break;
        default:
            accept = false;
            break;
    }

    // Update display if necessary
    if (refresh)
        postRedisplay();

    return accept;
}

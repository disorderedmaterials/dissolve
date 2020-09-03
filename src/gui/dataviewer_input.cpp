/*
    *** Data Viewer - Input
    *** src/gui/dataviewer_input.cpp
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
#include <QMenu>
#include <QMouseEvent>

// Mouse Moved
void DataViewer::mouseMoved(int dx, int dy)
{
    // If we are not actually interacting with the view, return now
    if (!interacting())
    {
        emit(currentCoordinateChanged());

        return;
    }

    auto refresh = false;

    // What we do here depends on the current mode
    switch (interactionMode())
    {
        case (DataViewer::ZoomToAreaInteraction):
            // No action to take - the selection box will be drawn from the clicked and current positions (already
            // stored)
            refresh = true;
            break;
        case (DataViewer::RotateViewInteraction):
            // Rotate view
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
        case (DataViewer::TranslateViewInteraction):
            // Turn off autofollow if it is currently on...
            if (view().autoFollowType() != View::NoAutoFollow)
            {
                view().setAutoFollowType(View::NoAutoFollow);
                emit(controlAspectChanged());
            }

            // If this is a flat view, shift the axis limits rather than translating the view
            if (view().isFlatView())
                view().shiftFlatAxisLimits(dx, dy);
            else
                view().translateView(dx / 15.0, dy / 15.0, 0.0);
            refresh = true;
            break;
        case (DataViewer::ZoomXRangeInteraction):
            // No action to take - the range will be drawn from the clicked and current positions (already stored)
            refresh = true;
            break;
        default:
            break;
    }

    // Notify any interested widgets that our current coordinate has changed
    emit(currentCoordinateChanged());

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
        emit(controlAspectChanged());
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
            //			i = clickedObjectInfo_.asInteger();
            // 			axesWindow_.updateAndShow();
            // 			axesWindow_.ui.AxesTabs->setCurrentIndex(i);
            // 			axesWindow_.ui.
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
    }

    // Reset clicked object info
    // 	clickedObject_ = NoObject;
    // 	clickedObjectInfo_.clear();
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
        Renderable *rend = renderableWithTag(queryObjectInfo());
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
                emit(controlAspectChanged());
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
            emit(controlAspectChanged());
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

/*
    *** Base Viewer - Interaction
    *** src/gui/viewer_interaction.cpp
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

#include "gui/viewer.hui"

/*
 * Protected Virtuals
 */

// Return text describing current interaction mode
const QString BaseViewer::interactionModeText() const { return "??? Unknown interaction mode"; }

// Start interaction
void BaseViewer::startInteraction() {}

// End interaction
void BaseViewer::endInteraction() {}

// Cancel current interaction
void BaseViewer::cancelInteraction() {}

/*
 * Public Functions
 */

// Set current interaction mode
void BaseViewer::setInteractionMode(int mode)
{
    // Cancel any current interaction
    cancelInteraction();

    interactionMode_ = mode;

    emit(interactionModeChanged());
}

// Return current interaction mode
int BaseViewer::interactionMode() const { return interactionMode_; }

// Return whether the viewer is currently being interacted with
bool BaseViewer::interacting() const { return interacting_; }

// Return clicked coordinate (in Axes frame) in 2D view
Vec3<double> BaseViewer::clicked2DAxesCoordinates() const { return clicked2DAxesCoordinates_; }

// Return current coordinate (in Axes frame) under mouse in 2D view
Vec3<double> BaseViewer::current2DAxesCoordinates() const { return current2DAxesCoordinates_; }

// Calculate 2D Axes coordinate from screen coordinates
Vec3<double> BaseViewer::screenTo2DAxes(int mouseX, int mouseY)
{
    // Must be a flat view in order to return 2D local coordinates
    if (!view_.isFlatView())
        return Vec3<double>();

    Vec3<double> result;
    if (view_.viewType() == View::FlatXYView)
        result.set(view_.screenToAxis(0, mouseX, mouseY, false), view_.screenToAxis(1, mouseX, mouseY, false), 0.0);
    else if (view_.viewType() == View::FlatXZView)
        result.set(view_.screenToAxis(0, mouseX, mouseY, false), 0.0, view_.screenToAxis(2, mouseX, mouseY, false));
    else if (view_.viewType() == View::FlatZYView)
        result.set(0.0, view_.screenToAxis(1, mouseX, mouseY, false), view_.screenToAxis(2, mouseX, mouseY, false));

    return result;
}

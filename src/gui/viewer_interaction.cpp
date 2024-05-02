// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/viewer.h"

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

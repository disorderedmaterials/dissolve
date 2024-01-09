// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/configurationViewer.hui"

// Perform post-initialisation operations
void ConfigurationViewer::postInitialiseGL() {}

// Perform post-resize operations
void ConfigurationViewer::postResizeGL() {}

// Render 2D overlay content
void ConfigurationViewer::render2DOverlay()
{
    /*
     * Draw interaction mode embellishments
     */
    glLoadIdentity();

    static LineStyle selectionBoxStyle(1.0, LineStipple::HalfDashStipple);

    if (interacting())
        switch (interactionMode())
        {
            default:
                break;
        }
}

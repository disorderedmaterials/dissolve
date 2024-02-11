// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/speciesViewer.h"

// Perform post-initialisation operations
void SpeciesViewer::postInitialiseGL() {}

// Perform post-resize operations
void SpeciesViewer::postResizeGL() {}

// Render 2D overlay content
void SpeciesViewer::render2DOverlay()
{
    /*
     * Draw interaction mode embellishments
     */
    glLoadIdentity();

    static LineStyle selectionBoxStyle(1.0, LineStipple::HalfDashStipple);

    if (interacting())
        switch (interactionMode())
        {
            case (SpeciesViewer::InteractionMode::SelectArea):
                // Draw dashed box indicating selection area, form clicked to current mouse coordinates
                selectionBoxStyle.sendToGL();
                glBegin(GL_LINE_LOOP);
                glVertex2d(rMouseDown_.x, rMouseDown_.y);
                glVertex2d(rMouseLast_.x, rMouseDown_.y);
                glVertex2d(rMouseLast_.x, rMouseLast_.y);
                glVertex2d(rMouseDown_.x, rMouseLast_.y);
                glEnd();
                break;
            default:
                break;
        }
}

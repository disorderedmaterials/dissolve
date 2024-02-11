// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/configurationViewer.h"
#include "gui/render/renderableConfiguration.h"
#include <QtGui/QMouseEvent>

// Mouse moved
void ConfigurationViewer::mouseMoved(int dx, int dy)
{
    // If we are not actually interacting with the view, return now
    if (!interacting())
        return;

    auto refresh = false;

    // What we do here depends on the current mode
    switch (transientInteractionMode_)
    {
        case (TransientInteractionMode::None):
            // End primary interaction
            switch (interactionMode_)
            {
                case (ConfigurationViewer::InteractionMode::Default):
                    // No action to take - the selection box will be drawn from the clicked and current positions (already
                    // stored)
                    refresh = true;
                    break;
                default:
                    break;
            }
            break;
        case (ConfigurationViewer::TransientInteractionMode::RotateView):
            // Rotate view
            if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
            {
            }
            else if (mouseDownModifiers_.testFlag(Qt::ControlModifier))
            {
            }
            else
            {
                view_.rotateView(-dy / 2.0, dx / 2.0);
                refresh = true;
            }
            break;
        case (ConfigurationViewer::TransientInteractionMode::TranslateView):
            // If this is a flat view, shift the axis limits rather than translating the view
            if (view_.isFlatView())
                view_.shiftFlatAxisLimits(dx, dy);
            else
                view_.translateView(dx / 15.0, dy / 15.0, 0.0);
            refresh = true;
            break;
        default:
            break;
    }

    if (refresh)
        postRedisplay();
}

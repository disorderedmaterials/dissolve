// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/configurationViewer.hui"
#include "gui/render/renderableConfiguration.h"

/*
 * Private Functions
 */

// Return atom at specified coordinates
const Atom *ConfigurationViewer::atomAt(int x, int y)
{
    if (!configuration_)
        return nullptr;

    double lengthScale;
    Vec3<double> rScreen;

    // Loop over atoms, converting the local coordinates into screen coordinates, and testing distance from the point
    // provided
    for (const auto &i : configuration_->atoms())
    {
        // Set the lengthscale to the appropriate atom radius for the current display style - it will be replaced with
        // the atom's screen radius
        lengthScale = 0.3;
        rScreen = view_.dataToScreen(i.r(), lengthScale);

        // Subtract the reference coordinates and test against the screen radius
        rScreen.x -= x;
        rScreen.y -= y;
        if (sqrt(rScreen.x * rScreen.x + rScreen.y * rScreen.y) < lengthScale)
            return &i;
    }

    return nullptr;
}

/*
 * Protected Virtuals
 */

// Start interaction at the specified screen coordinates
void ConfigurationViewer::startInteraction()
{
    // Check the button that started the interaction
    if (buttonState_.testFlag(Qt::LeftButton))
    {
        switch (interactionMode_)
        {
            // Selection
            case (ConfigurationViewer::InteractionMode::Default):
                break;
            default:
                fmt::print("Unhandled primary mode {} in ConfigurationViewer::startInteraction().\n", (int)interactionMode_);
                break;
        }
    }
    else if (buttonState_.testFlag(Qt::RightButton))
        transientInteractionMode_ = ConfigurationViewer::TransientInteractionMode::RotateView;
    else if (buttonState_.testFlag(Qt::MiddleButton))
        transientInteractionMode_ = ConfigurationViewer::TransientInteractionMode::TranslateView;
}

// End interaction at the specified screen coordinates
void ConfigurationViewer::endInteraction()
{
    // Finalise interaction type
    switch (transientInteractionMode_)
    {
        case (TransientInteractionMode::None):
            // End primary interaction
            switch (interactionMode_)
            {
                case (ConfigurationViewer::InteractionMode::Default):
                    break;
                default:
                    fmt::print("Unhandled primary mode {} in ConfigurationViewer::endInteraction().\n", (int)interactionMode_);
                    break;
            }
            break;
        case (ConfigurationViewer::TransientInteractionMode::RotateView):
        case (ConfigurationViewer::TransientInteractionMode::TranslateView):
            // Rotation / translation has already been modified, so nothing more to do
            break;
        default:
            fmt::print("Unhandled secondary mode {} in ConfigurationViewer::endInteraction().\n",
                       (int)transientInteractionMode_);
            break;
    }

    // Reset any transient interaction
    transientInteractionMode_ = TransientInteractionMode::None;
}

// Cancel current interaction
void ConfigurationViewer::cancelInteraction()
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
void ConfigurationViewer::setInteractionMode(ConfigurationViewer::InteractionMode mode)
{
    // Cancel any current interaction
    cancelInteraction();

    interactionMode_ = mode;

    emit(interactionModeChanged());
}

// Return current interaction mode
ConfigurationViewer::InteractionMode ConfigurationViewer::interactionMode() const { return interactionMode_; }

// Return text describing current interaction mode
const QString ConfigurationViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (ConfigurationViewer::InteractionMode::Default):
            return "<b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        default:
            return "Unknown ConfigurationViewerInteraction";
    }
}

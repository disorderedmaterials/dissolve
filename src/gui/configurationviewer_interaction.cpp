// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/configurationviewer.hui"
#include "gui/render/renderableconfiguration.h"

/*
 * Private Functions
 */

// Return atom at specified coordinates
const std::shared_ptr<Atom> ConfigurationViewer::atomAt(int x, int y)
{
    if (!configuration_)
        return nullptr;

    double lengthScale;
    Vec3<double> rScreen;

    // Loop over atoms, converting the local coordinates into screen coordinates, and testing distance from the point
    // provided
    for (auto i : configuration_->constAtoms())
    {
        // Set the lengthscale to the appropriate atom radius for the current display style - it will be replaced with
        // the atom's screen radius
        lengthScale = 0.3;
        rScreen = view_.dataToScreen(i->r(), lengthScale);

        // Subtract the reference coordinates and test against the screen radius
        rScreen.x -= x;
        rScreen.y -= y;
        if (sqrt(rScreen.x * rScreen.x + rScreen.y * rScreen.y) < lengthScale)
            return i;
    }

    return nullptr;
}

/*
 * Protected Virtuals
 */

// Start interaction at the specified screen coordinates
void ConfigurationViewer::startInteraction()
{
    switch (interactionMode())
    {
        // Default Interaction Mode
        case (ConfigurationViewer::DefaultInteraction):
            // This is the standard mode, giving access to view manipulation
            if (buttonState_.testFlag(Qt::RightButton))
                setInteractionMode(ConfigurationViewer::RotateViewInteraction);
            else if (buttonState_.testFlag(Qt::MiddleButton))
                setInteractionMode(ConfigurationViewer::TranslateViewInteraction);
            break;
        default:
            break;
    }
}

// End interaction at the specified screen coordinates
void ConfigurationViewer::endInteraction()
{
    // Finalise interaction type
    switch (interactionMode())
    {
        case (ConfigurationViewer::DefaultInteraction):
            break;
        case (ConfigurationViewer::RotateViewInteraction):
            // Rotation matrix has already been modified. Revert to default interaction mode
            setInteractionMode(ConfigurationViewer::DefaultInteraction);
            break;
        case (ConfigurationViewer::TranslateViewInteraction):
            // Translation has already been applied. Revert to default interaction mode
            setInteractionMode(ConfigurationViewer::DefaultInteraction);
            break;
        default:
            Messenger::error("Don't know how to complete interaction mode {}\n", interactionMode());
            break;
    }
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

// Return text describing current interaction mode
const QString ConfigurationViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (ConfigurationViewer::DefaultInteraction):
            return "<b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        case (ConfigurationViewer::RotateViewInteraction):
            return "Rotate view";
        case (ConfigurationViewer::TranslateViewInteraction):
            return "Translate";
        default:
            return "Unknown ConfigurationViewerInteraction";
    }
}

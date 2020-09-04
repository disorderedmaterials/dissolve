/*
    *** Configuration Viewer - Interaction
    *** src/gui/configurationviewer_interaction.cpp
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

#include "classes/configuration.h"
#include "gui/configurationviewer.hui"
#include "gui/render/renderableconfiguration.h"

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
    const DynamicArray<Atom> &atoms = configuration_->constAtoms();
    for (int n = 0; n < atoms.nItems(); ++n)
    {
        const Atom *i = atoms.constValue(n);

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

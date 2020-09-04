/*
    *** Species Viewer - Interaction
    *** src/gui/siteviewer_interaction.cpp
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

#include "classes/species.h"
#include "classes/speciesatom.h"
#include "gui/render/renderablespecies.h"
#include "gui/siteviewer.hui"

/*
 * Private Functions
 */

// Return atom at specified coordinates
SpeciesAtom *SiteViewer::atomAt(int x, int y)
{
    if (!species_)
        return nullptr;

    double lengthScale;
    Vec3<double> rScreen;

    // Loop over atoms, converting the local coordinates into screen coordinates, and testing distance from the point
    // provided
    ListIterator<SpeciesAtom> atomIterator(species_->atoms());
    while (SpeciesAtom *i = atomIterator.iterate())
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
void SiteViewer::startInteraction()
{
    switch (interactionMode())
    {
        // Default Interaction Mode
        case (SiteViewer::DefaultInteraction):
            // This is the standard mode, giving access to view manipulation
            if (buttonState_.testFlag(Qt::LeftButton))
                setInteractionMode(SiteViewer::SelectAreaInteraction);
            else if (buttonState_.testFlag(Qt::RightButton))
                setInteractionMode(SiteViewer::RotateViewInteraction);
            else if (buttonState_.testFlag(Qt::MiddleButton))
                setInteractionMode(SiteViewer::TranslateViewInteraction);
            break;
        default:
            break;
    }
}

// End interaction at the specified screen coordinates
void SiteViewer::endInteraction()
{
    SpeciesAtom *i, *j;

    // Finalise interaction type
    switch (interactionMode())
    {
        case (SiteViewer::DefaultInteraction):
            break;
        case (SiteViewer::SelectAreaInteraction):
            // Check the pixel area of the clicked region and determine whether this was actually a targeted click
            // rather than an area select
            if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
            {
                // Single, targetted click - atom under mouse?
                SpeciesAtom *i = atomAt(rMouseLast_.x, rMouseLast_.y);

                // If there is an atom at the current position, (de)select it, maintaining the current selection
                // if Shift was pressed
                if (i)
                {
                    if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
                        species_->toggleAtomSelection(i);
                    else
                    {
                        species_->clearAtomSelection();
                        species_->selectAtom(i);
                    }
                }
                else
                {
                    // No atom at the current position - if Shift was not pressed, deselect everything
                    if (!mouseDownModifiers_.testFlag(Qt::ShiftModifier))
                        species_->clearAtomSelection();
                }
            }
            else
            {
                // Click-drag - area select
                // If Shift was not pressed, clear the current selection first
                if (!mouseDownModifiers_.testFlag(Qt::ShiftModifier))
                    species_->clearAtomSelection();
                Vec3<double> rScreen;
                QRect selectionRect(QPoint(rMouseDown_.x, rMouseDown_.y), QPoint(rMouseLast_.x, rMouseLast_.y));
                ListIterator<SpeciesAtom> atomIterator(species_->atoms());
                while (SpeciesAtom *i = atomIterator.iterate())
                {
                    rScreen = view_.dataToScreen(i->r());
                    if (selectionRect.contains(rScreen.x, rScreen.y))
                        species_->selectAtom(i);
                }
            }

            // Update the selection
            speciesRenderable_->recreateSelectionPrimitive();
            emit(atomSelectionChanged());

            // Revert to default interaction mode
            setInteractionMode(SiteViewer::DefaultInteraction);
            break;
        case (SiteViewer::RotateViewInteraction):
            // Rotation matrix has already been modified. Revert to default interaction mode
            setInteractionMode(SiteViewer::DefaultInteraction);
            break;
        case (SiteViewer::TranslateViewInteraction):
            // Translation has already been applied. Revert to default interaction mode
            setInteractionMode(SiteViewer::DefaultInteraction);
            break;
        default:
            Messenger::error("Don't know how to complete interaction mode {}\n", interactionMode());
            break;
    }
}

// Cancel current interaction
void SiteViewer::cancelInteraction()
{
    // Perform any actions necessary to properly cancel the current interaction
    switch (interactionMode())
    {
        default:
            break;
    }

    // Reset other data
    clickedAtom_ = nullptr;
}

/*
 * Public Functions
 */

// Return text describing current interaction mode
const QString SiteViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (SiteViewer::DefaultInteraction):
            return "View: <b>Left</b> Select; <b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        case (SiteViewer::SelectAreaInteraction):
            return "Select atoms: <b>Left-Click</b> Select individual atoms; <b>Left-Click-Drag</b> Area select; "
                   "<i>+Shift</i> Toggle";
        case (SiteViewer::RotateViewInteraction):
            return "Rotate view";
        case (SiteViewer::TranslateViewInteraction):
            return "Translate";
        default:
            return "Unknown SiteViewerInteraction";
    }
}

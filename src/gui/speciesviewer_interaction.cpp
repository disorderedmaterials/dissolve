/*
    *** Species Viewer - Interaction
    *** src/gui/speciesviewer_interaction.cpp
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
#include "gui/speciesviewer.hui"

/*
 * Private Functions
 */

// Return atom at specified coordinates
SpeciesAtom *SpeciesViewer::atomAt(int x, int y)
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
void SpeciesViewer::startInteraction()
{
    switch (interactionMode())
    {
        // Default Interaction Mode
        case (SpeciesViewer::DefaultInteraction):
            // This is the standard mode, giving access to view manipulation
            if (buttonState_.testFlag(Qt::LeftButton))
                setInteractionMode(SpeciesViewer::SelectAreaInteraction);
            else if (buttonState_.testFlag(Qt::RightButton))
                setInteractionMode(SpeciesViewer::RotateViewInteraction);
            else if (buttonState_.testFlag(Qt::MiddleButton))
                setInteractionMode(SpeciesViewer::TranslateViewInteraction);
            break;
        // Draw
        case (SpeciesViewer::DrawInteraction):
            // If the left mouse button is not flagged, do nothing
            if (!buttonState_.testFlag(Qt::LeftButton))
                break;

            // Set starting atom (if there is one at the current position)
            clickedAtom_ = atomAt(rMouseDown_.x, rMouseDown_.y);

            // Get the clicked position in the coordinate space of the Species
            drawCoordinateStart_ = clickedAtom_ ? clickedAtom_->r() : view().screenToData(rMouseDown_.x, rMouseDown_.y, 0.0);
            drawCoordinateCurrent_ = drawCoordinateStart_;

            // If Ctrl is pressed, we transmute the atom under the mouse, but don't allow drawing of anything else
            if (clickedAtom_ && mouseDownModifiers_.testFlag(Qt::ControlModifier))
                break;

            // Update the interaction Primitive
            if (clickedAtom_)
                speciesRenderable_->recreateDrawInteractionPrimitive(clickedAtom_, drawCoordinateCurrent_, drawElement_);
            else
                speciesRenderable_->recreateDrawInteractionPrimitive(drawCoordinateStart_, drawElement_, drawCoordinateCurrent_,
                                                                     drawElement_);
            break;
        // Delete
        case (SpeciesViewer::DeleteInteraction):
            // If the left mouse button is not flagged, do nothing
            if (!buttonState_.testFlag(Qt::LeftButton))
                break;

            // Set starting atom (if there is one at the current position)
            clickedAtom_ = atomAt(rMouseDown_.x, rMouseDown_.y);

            // Get the clicked position in the coordinate space of the Species
            drawCoordinateStart_ = clickedAtom_ ? clickedAtom_->r() : view().screenToData(rMouseDown_.x, rMouseDown_.y, 0.0);
            drawCoordinateCurrent_ = drawCoordinateStart_;

            // Update the interaction Primitive
            if (clickedAtom_)
                speciesRenderable_->recreateDrawInteractionPrimitive(clickedAtom_, drawCoordinateCurrent_, drawElement_);
            else
                speciesRenderable_->recreateDrawInteractionPrimitive(drawCoordinateStart_, drawElement_, drawCoordinateCurrent_,
                                                                     drawElement_);
            break;
        default:
            break;
    }
}

// End interaction at the specified screen coordinates
void SpeciesViewer::endInteraction()
{
    SpeciesAtom *i, *j;

    // Finalise interaction type
    switch (interactionMode())
    {
        case (SpeciesViewer::DefaultInteraction):
            break;
        case (SpeciesViewer::SelectAreaInteraction):
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
            setInteractionMode(SpeciesViewer::DefaultInteraction);
            break;
        case (SpeciesViewer::RotateViewInteraction):
            // Rotation matrix has already been modified. Revert to default interaction mode
            setInteractionMode(SpeciesViewer::DefaultInteraction);
            break;
        case (SpeciesViewer::TranslateViewInteraction):
            // Translation has already been applied. Revert to default interaction mode
            setInteractionMode(SpeciesViewer::DefaultInteraction);
            break;
        case (SpeciesViewer::DrawInteraction):
            // If the left mouse button is not flagged, do nothing
            if (!buttonState_.testFlag(Qt::LeftButton))
                break;

            // If Ctrl was pressed, and an atom was clicked, transmute it to the current element
            if (mouseDownModifiers_.testFlag(Qt::ControlModifier))
            {
                if (clickedAtom_)
                {
                    species_->transmuteAtom(clickedAtom_, drawElement_);

                    // Notify that the data has changed
                    emit(dataModified());

                    // Update display
                    postRedisplay();
                }

                // We're done - nothing more to add
                break;
            }

            // If an atom was not clicked at the start of the interaction, create a new one now
            i = clickedAtom_ ? clickedAtom_ : species_->addAtom(drawElement_, drawCoordinateStart_);

            // Get atom at current coordinates - if there isn't one, create one now
            j = atomAt(rMouseLast_.x, rMouseLast_.y);
            if (!j)
                j = species_->addAtom(drawElement_, drawCoordinateCurrent_);

            // Create a bond between the two atoms, if one doesn't currently exist
            if ((i != j) && (!species_->hasBond(i, j)))
                species_->addBond(i, j);

            // Clear the interaction Primitive
            speciesRenderable_->clearInteractionPrimitive();

            // Notify that the data has changed
            emit(dataModified());

            // Update display
            postRedisplay();
            break;
        case (SpeciesViewer::DeleteInteraction):
            // If the left mouse button is not flagged, do nothing
            if (!buttonState_.testFlag(Qt::LeftButton))
                break;

            // If an atom was not clicked at the start of the interaction, break now
            if (!clickedAtom_)
                break;

            // Get atom at current coordinates - if its the same as the clicked atom, nullify the pointer so we
            // don't try to remove a bonv
            j = atomAt(rMouseLast_.x, rMouseLast_.y);
            if (j == clickedAtom_)
                j = nullptr;

            // If there is an atom 'j', search for the bond to delete. Otherwise, delete clickedAtom_
            if (!j)
                species_->removeAtom(clickedAtom_);
            else if (species_->hasBond(clickedAtom_, j))
                species_->removeBond(clickedAtom_, j);

            // Clear the interaction Primitive
            speciesRenderable_->clearInteractionPrimitive();

            // Notify that the data has changed
            emit(dataModified());

            // Update display
            postRedisplay();
            break;
        default:
            Messenger::error("Don't know how to complete interaction mode {}\n", interactionMode());
            break;
    }
}

// Cancel current interaction
void SpeciesViewer::cancelInteraction()
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
const QString SpeciesViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (SpeciesViewer::DefaultInteraction):
            return "View: <b>Left</b> Select; <b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        case (SpeciesViewer::SelectAreaInteraction):
            return "Select atoms: <b>Left-Click</b> Select individual atoms; <b>Left-Click-Drag</b> Area select; "
                   "<i>+Shift</i> Toggle";
        case (SpeciesViewer::RotateViewInteraction):
            return "Rotate view";
        case (SpeciesViewer::TranslateViewInteraction):
            return "Translate";
        case (SpeciesViewer::DrawInteraction):
            return "Draw: <b>Left</b> Draw [+<i>Ctrl</i>: Transmute]; <b>Right</b> Rotate view";
        case (SpeciesViewer::DeleteInteraction):
            return "Delete: <b>Left-Click</b> Delete Atom; <b>Left-Click-Drag</b> Delete Bond";
        default:
            return "Unknown SpeciesViewerInteraction";
    }
}

// Set current Element for DrawInteraction
void SpeciesViewer::setDrawElement(Element *element) { drawElement_ = element; }

// Return current Element for DrawInteraction
Element *SpeciesViewer::drawElement() const { return drawElement_; }

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/speciesAtom.h"
#include "gui/render/renderableSpecies.h"
#include "gui/speciesViewer.hui"

/*
 * Private Functions
 */

// Return atom at specified coordinates
std::optional<int> SpeciesViewer::atomIndexAt(int x, int y)
{
    if (!species_)
        return {};

    double lengthScale;
    Vec3<double> rScreen;

    // Loop over atoms, converting the local coordinates into screen coordinates, and testing distance from the point
    // provided
    for (auto &i : species_->atoms())
    {
        // Set the lengthscale to the appropriate atom radius for the current display style - it will be replaced with
        // the atom's screen radius
        lengthScale = 0.3;
        rScreen = view_.dataToScreen(i.r(), lengthScale);

        // Subtract the reference coordinates and test against the screen radius
        rScreen.x -= x;
        rScreen.y -= y;
        if (sqrt(rScreen.x * rScreen.x + rScreen.y * rScreen.y) < lengthScale)
            return i.index();
    }

    return {};
}

/*
 * Protected Virtuals
 */

// Start interaction at the specified screen coordinates
void SpeciesViewer::startInteraction()
{
    // Check the button that started the interaction
    if (buttonState_.testFlag(Qt::LeftButton))
    {
        switch (interactionMode_)
        {
            // Selection
            case (SpeciesViewer::InteractionMode::Select):
                setInteractionMode(SpeciesViewer::InteractionMode::SelectArea);
                break;
            // Draw
            case (SpeciesViewer::InteractionMode::Draw):
                // Set starting atom (if there is one at the current position)
                clickedAtomIndex_ = atomIndexAt(rMouseDown_.x, rMouseDown_.y);

                // Get the clicked position in the coordinate space of the Species
                drawCoordinateStart_ = clickedAtomIndex_ ? species_->atom(clickedAtomIndex_.value()).r()
                                                         : view().screenToData(rMouseDown_.x, rMouseDown_.y, 0.0);
                drawCoordinateCurrent_ = drawCoordinateStart_;

                // If Ctrl is pressed, we transmute the atom under the mouse, but don't allow drawing of anything else
                if (clickedAtomIndex_ && mouseDownModifiers_.testFlag(Qt::ControlModifier))
                    break;

                // Update the interaction Primitive
                if (clickedAtomIndex_)
                    speciesRenderable_->recreateDrawInteractionPrimitive(&species_->atom(clickedAtomIndex_.value()),
                                                                         drawCoordinateCurrent_, drawElement_);
                else
                    speciesRenderable_->recreateDrawInteractionPrimitive(drawCoordinateStart_, drawElement_,
                                                                         drawCoordinateCurrent_, drawElement_);
                break;
            // Delete
            case (SpeciesViewer::InteractionMode::Delete):
                // Set starting atom (if there is one at the current position)
                clickedAtomIndex_ = atomIndexAt(rMouseDown_.x, rMouseDown_.y);

                // Get the clicked position in the coordinate space of the Species
                drawCoordinateStart_ = clickedAtomIndex_ ? species_->atom(clickedAtomIndex_.value()).r()
                                                         : view().screenToData(rMouseDown_.x, rMouseDown_.y, 0.0);
                drawCoordinateCurrent_ = drawCoordinateStart_;

                // Update the interaction Primitive
                if (clickedAtomIndex_)
                    speciesRenderable_->recreateDeleteInteractionPrimitive(&species_->atom(clickedAtomIndex_.value()));
                else
                    speciesRenderable_->clearInteractionPrimitive();
                break;
            default:
                fmt::print("Unhandled primary mode {} in SpeciesViewer::startInteraction().\n", (int)interactionMode_);
                break;
        }
    }
    else if (buttonState_.testFlag(Qt::RightButton))
        transientInteractionMode_ = SpeciesViewer::TransientInteractionMode::RotateView;
    else if (buttonState_.testFlag(Qt::MiddleButton))
        transientInteractionMode_ = SpeciesViewer::TransientInteractionMode::TranslateView;
}

// End interaction at the specified screen coordinates
void SpeciesViewer::endInteraction()
{
    int idI, idJ;
    std::optional<int> currentAtomIndex;

    // Finalise interaction type
    switch (transientInteractionMode_)
    {
        case (TransientInteractionMode::None):
            // End primary interaction
            switch (interactionMode_)
            {
                case (SpeciesViewer::InteractionMode::Select):
                    break;
                case (SpeciesViewer::InteractionMode::SelectArea):
                    // Check the pixel area of the clicked region and determine whether this was actually a targeted click
                    // rather than an area select
                    if ((rMouseDown_ - rMouseLast_).magnitude() < 9.0)
                    {
                        // Single, targetted click - atom under mouse?
                        auto clickedIndex = atomIndexAt(rMouseLast_.x, rMouseLast_.y);

                        // If there is an atom at the current position, (de)select it, maintaining the current selection
                        // if Shift was pressed
                        if (clickedIndex)
                        {
                            if (mouseDownModifiers_.testFlag(Qt::ShiftModifier))
                                species_->toggleAtomSelection(clickedIndex.value());
                            else
                            {
                                species_->clearAtomSelection();
                                species_->selectAtom(clickedIndex.value());
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
                        for (auto &i : species_->atoms())
                        {
                            rScreen = view_.dataToScreen(i.r());
                            if (selectionRect.contains(rScreen.x, rScreen.y))
                                species_->selectAtom(i.index());
                        }
                    }

                    // Update the selection
                    speciesRenderable_->recreateSelectionPrimitive();
                    emit(atomsChanged());

                    // Revert to default interaction mode
                    setInteractionMode(SpeciesViewer::InteractionMode::Select);
                    break;
                case (SpeciesViewer::InteractionMode::Draw):
                    // If the left mouse button is not flagged, do nothing
                    if (!buttonState_.testFlag(Qt::LeftButton))
                        break;

                    // If Ctrl was pressed, and an atom was clicked, transmute it to the current element
                    if (mouseDownModifiers_.testFlag(Qt::ControlModifier))
                    {
                        if (clickedAtomIndex_)
                        {
                            species_->transmuteAtom(clickedAtomIndex_.value(), drawElement_);

                            // Notify that the data has changed
                            emit(dataModified());

                            // Update display
                            postRedisplay();
                        }

                        // We're done - nothing more to add
                        break;
                    }

                    // If an atom was not clicked at the start of the interaction, create a new one now
                    idI = clickedAtomIndex_ ? clickedAtomIndex_.value() : species_->addAtom(drawElement_, drawCoordinateStart_);

                    // Get atom at current coordinates - if there isn't one, create one now
                    currentAtomIndex = atomIndexAt(rMouseLast_.x, rMouseLast_.y);
                    idJ = currentAtomIndex ? currentAtomIndex.value() : species_->addAtom(drawElement_, drawCoordinateCurrent_);

                    // Create a bond between the two atoms, if one doesn't currently exist
                    if ((idI != idJ) && (!species_->hasBond(idI, idJ)))
                        species_->addBond(idI, idJ);

                    // Clear the interaction Primitive
                    speciesRenderable_->clearInteractionPrimitive();

                    // Notify that the data has changed
                    emit(dataModified());
                    emit(atomsChanged());

                    // Update display
                    postRedisplay();
                    break;
                case (SpeciesViewer::InteractionMode::Delete):
                    // If the left mouse button is not flagged, do nothing
                    if (!buttonState_.testFlag(Qt::LeftButton))
                        break;

                    // If an atom was not clicked at the start of the interaction, break now
                    if (!clickedAtomIndex_)
                        break;

                    // Get atom at current coordinates - if it's the same as the clicked atom, reset the stored index
                    currentAtomIndex = atomIndexAt(rMouseLast_.x, rMouseLast_.y);
                    if (currentAtomIndex && currentAtomIndex.value() == clickedAtomIndex_)
                        currentAtomIndex = std::nullopt;

                    // If there is an atom 'j', search for the bond to delete. Otherwise, delete clickedAtomIndex_
                    if (!currentAtomIndex)
                        species_->removeAtom(clickedAtomIndex_.value());
                    else if (species_->hasBond(clickedAtomIndex_.value(), currentAtomIndex.value()))
                        species_->removeBond(clickedAtomIndex_.value(), currentAtomIndex.value());

                    // Clear the interaction Primitive
                    speciesRenderable_->clearInteractionPrimitive();

                    // Notify that the data has changed
                    emit(dataModified());
                    emit(atomsChanged());

                    // Update display
                    postRedisplay();
                    break;
                default:
                    fmt::print("Unhandled primary mode {} in SpeciesViewer::endInteraction().\n", (int)interactionMode_);
                    break;
            }
            break;
        case (SpeciesViewer::TransientInteractionMode::RotateView):
        case (SpeciesViewer::TransientInteractionMode::TranslateView):
            // Rotation / translation has already been modified, so nothing more to do
            break;
        default:
            fmt::print("Unhandled secondary mode {} in SpeciesViewer::endInteraction().\n", (int)transientInteractionMode_);
            break;
    }

    // Reset any transient interaction
    transientInteractionMode_ = TransientInteractionMode::None;
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
    clickedAtomIndex_ = std::nullopt;
}

/*
 * Public Functions
 */

// Set current interaction mode
void SpeciesViewer::setInteractionMode(SpeciesViewer::InteractionMode mode)
{
    // Cancel any current interaction
    cancelInteraction();

    interactionMode_ = mode;

    emit(interactionModeChanged());
}

// Return current interaction mode
SpeciesViewer::InteractionMode SpeciesViewer::interactionMode() const { return interactionMode_; }

// Return text describing current interaction mode
const QString SpeciesViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (SpeciesViewer::InteractionMode::Select):
            return "View: <b>Left</b> Select; <b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        case (SpeciesViewer::InteractionMode::SelectArea):
            return "Select atoms: <b>Left-Click</b> Select individual atoms; <b>Left-Click-Drag</b> Area select; "
                   "<i>+Shift</i> Toggle";
        case (SpeciesViewer::InteractionMode::Draw):
            return "Draw: <b>Left</b> Draw [+<i>Ctrl</i>: Transmute]; <b>Right</b> Rotate view";
        case (SpeciesViewer::InteractionMode::Delete):
            return "Delete: <b>Left-Click</b> Delete Atom; <b>Left-Click-Drag</b> Delete Bond";
        default:
            return "Unknown SpeciesViewerInteraction";
    }
}

// Set current Element for DrawInteraction
void SpeciesViewer::setDrawElement(Elements::Element Z) { drawElement_ = Z; }

// Return current Element for DrawInteraction
Elements::Element SpeciesViewer::drawElement() const { return drawElement_; }

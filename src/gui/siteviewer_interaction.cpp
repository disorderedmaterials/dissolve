// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
            return &i;
    }

    return nullptr;
}

/*
 * Protected Virtuals
 */

// Start interaction at the specified screen coordinates
void SiteViewer::startInteraction()
{
    // Check the button that started the interaction
    if (buttonState_.testFlag(Qt::LeftButton))
    {
        switch (interactionMode_)
        {
            // Default Interaction Mode
            case (SiteViewer::InteractionMode::Select):
                setInteractionMode(SiteViewer::InteractionMode::SelectArea);
                break;
            default:
                fmt::print("Unhandled primary mode {} in SiteViewer::startInteraction().\n", interactionMode_);
                break;
        }
    }
    else if (buttonState_.testFlag(Qt::RightButton))
        transientInteractionMode_ = SiteViewer::TransientInteractionMode::RotateView;
    else if (buttonState_.testFlag(Qt::MiddleButton))
        transientInteractionMode_ = SiteViewer::TransientInteractionMode::TranslateView;
}

// End interaction at the specified screen coordinates
void SiteViewer::endInteraction()
{
    // Finalise interaction type
    switch (transientInteractionMode_)
    {
        case (TransientInteractionMode::None):
            // End primary interaction
            switch (interactionMode_)
            {
                case (SiteViewer::InteractionMode::Select):
                    break;
                case (SiteViewer::InteractionMode::SelectArea):
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
                        for (auto &i : species_->atoms())
                        {
                            rScreen = view_.dataToScreen(i.r());
                            if (selectionRect.contains(rScreen.x, rScreen.y))
                                species_->selectAtom(&i);
                        }
                    }

                    // Update the selection
                    speciesRenderable_->recreateSelectionPrimitive();
                    emit(atomsChanged());

                    // Revert to default interaction mode
                    setInteractionMode(SiteViewer::InteractionMode::Select);
                    break;
                default:
                    fmt::print("Unhandled primary mode {} in SiteViewer::endInteraction().\n", interactionMode_);
                    break;
            }
            break;
        case (SiteViewer::TransientInteractionMode::RotateView):
        case (SiteViewer::TransientInteractionMode::TranslateView):
            // Rotation / translation has already been modified, so nothing more to do
            break;
        default:
            fmt::print("Unhandled secondary mode {} in SiteViewer::endInteraction().\n", transientInteractionMode_);
            break;
    }

    // Reset any transient interaction
    transientInteractionMode_ = TransientInteractionMode::None;
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

// Set current interaction mode
void SiteViewer::setInteractionMode(SiteViewer::InteractionMode mode)
{
    // Cancel any current interaction
    cancelInteraction();

    interactionMode_ = mode;

    emit(interactionModeChanged());
}

// Return current interaction mode
SiteViewer::InteractionMode SiteViewer::interactionMode() const { return interactionMode_; }

// Return text describing current interaction mode
const QString SiteViewer::interactionModeText() const
{
    switch (interactionMode())
    {
        case (SiteViewer::InteractionMode::Select):
            return "View: <b>Left</b> Select; <b>Right</b> Rotate; <b>Middle</b> Translate; <b>Wheel</b> Zoom";
        case (SiteViewer::InteractionMode::SelectArea):
            return "Select atoms: <b>Left-Click</b> Select individual atoms; <b>Left-Click-Drag</b> Area select; "
                   "<i>+Shift</i> Toggle";
        default:
            return "Unknown SiteViewerInteraction";
    }
}

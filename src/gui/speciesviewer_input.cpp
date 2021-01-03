// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/render/renderablespecies.h"
#include "gui/speciesviewer.hui"
#include <QtGui/QMouseEvent>

// Mouse moved
void SpeciesViewer::mouseMoved(int dx, int dy)
{
    // If we are not actually interacting with the view, return now
    if (!interacting())
        return;

    auto refresh = false;
    SpeciesAtom *currentAtom = nullptr;

    // What we do here depends on the current mode
    switch (interactionMode())
    {
        case (SpeciesViewer::SelectAreaInteraction):
            // No action to take - the selection box will be drawn from the clicked and current positions (already
            // stored)
            refresh = true;
            break;
        case (SpeciesViewer::RotateViewInteraction):
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
        case (SpeciesViewer::TranslateViewInteraction):
            // If this is a flat view, shift the axis limits rather than translating the view
            if (view_.isFlatView())
                view_.shiftFlatAxisLimits(dx, dy);
            else
                view_.translateView(dx / 15.0, dy / 15.0, 0.0);
            refresh = true;
            break;
        case (SpeciesViewer::DrawInteraction):
            if (buttonState_.testFlag(Qt::LeftButton))
            {
                // Get atom at the current position (if any)
                currentAtom = atomAt(rMouseLast_.x, rMouseLast_.y);

                // Set the current drawing coordinates in data-space
                drawCoordinateCurrent_ =
                    currentAtom ? currentAtom->r() : view().screenToData(rMouseLast_.x, rMouseLast_.y, 0.0);

                // Update the interaction Primitive
                if (clickedAtom_)
                {
                    if (currentAtom)
                        speciesRenderable_->recreateDrawInteractionPrimitive(clickedAtom_, currentAtom);
                    else
                        speciesRenderable_->recreateDrawInteractionPrimitive(clickedAtom_, drawCoordinateCurrent_,
                                                                             drawElement_);
                }
                else
                    speciesRenderable_->recreateDrawInteractionPrimitive(drawCoordinateStart_, drawElement_,
                                                                         drawCoordinateCurrent_, drawElement_);
            }
            else if (buttonState_.testFlag(Qt::RightButton))
                view_.rotateView(-dy / 2.0, dx / 2.0);

            refresh = true;
            break;
        default:
            break;
    }

    if (refresh)
        postRedisplay();
}

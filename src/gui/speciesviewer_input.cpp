// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/render/renderablespecies.h"
#include "gui/speciesviewer.hui"
#include "neta/neta.h"
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
    switch (transientInteractionMode_)
    {
        case (TransientInteractionMode::None):
            // End primary interaction
            switch (interactionMode_)
            {
                case (SpeciesViewer::InteractionMode::SelectArea):
                    // No action to take - the selection box will be drawn from the clicked and current positions (already
                    // stored)
                    refresh = true;
                    break;
                case (SpeciesViewer::InteractionMode::Draw):
                    // If the left button is pressed, need to update our interaction primitives
                    if (!buttonState_.testFlag(Qt::LeftButton))
                        break;

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
                    refresh = true;
                    break;
                case (SpeciesViewer::InteractionMode::Delete):
                    // If the left button is pressed, need to update our interaction primitives
                    if (!buttonState_.testFlag(Qt::LeftButton))
                        break;

                    // Update the interaction Primitive
                    if (clickedAtom_)
                        speciesRenderable_->recreateDeleteInteractionPrimitive(clickedAtom_,
                                                                               atomAt(rMouseLast_.x, rMouseLast_.y));
                    else
                        speciesRenderable_->clearInteractionPrimitive();

                    refresh = true;
                    break;
                default:
                    break;
            }
            break;
        case (SpeciesViewer::TransientInteractionMode::RotateView):
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
        case (SpeciesViewer::TransientInteractionMode::TranslateView):
            // If this is a flat view, shift the axis limits rather than translating the view
            if (view_.isFlatView())
                view_.shiftFlatAxisLimits(dx, dy);
            else
                view_.translateView(dx / 15.0, dy / 15.0, 0.0);
            refresh = true;
            break;
    }

    if (refresh)
        postRedisplay();
}

// Context menu requested
void SpeciesViewer::contextMenuRequested(QPoint pos)
{
    QMenu menu(this);
    //    menu.setFont(font());

    // Reset View
    auto *resetViewAction = menu.addAction("&Reset View");

    // Copy to clipboard
    auto *copyToClipboardAction = menu.addAction("&Copy to clipboard");

    // Atom selection context menu?
    QAction *selectSimilarDirect = nullptr, *selectSimilarPrimary = nullptr, *selectSimilarSecondary = nullptr;
    if (species_ && species_->nSelectedAtoms() > 0)
    {
        menu.addSection("Current Selection");

        // Atom select submenu
        auto *selectMenu = menu.addMenu("Select similar atoms...");
        selectMenu->setEnabled(species_->nSelectedAtoms() == 1);
        selectSimilarDirect = selectMenu->addAction("By direct connectivity");
        selectSimilarPrimary = selectMenu->addAction("Based on primary neighbours");
        selectSimilarSecondary = selectMenu->addAction("Based on primary and secondary neighbours");
    }

    // Execute the menu
    auto *selectedAction = menu.exec(mapToGlobal(pos));
    if (selectedAction == nullptr)
    {
        cancelInteraction();
        return;
    }

    // Act on the action!
    if (selectedAction == resetViewAction)
        view_.resetViewMatrix();
    else if (selectedAction == copyToClipboardAction)
        copyViewToClipboard(true);
    else if (selectedAction == selectSimilarDirect || selectedAction == selectSimilarPrimary ||
             selectedAction == selectSimilarSecondary)
    {
        // Create a NETA description from the current atom
        NETADefinition neta(species_->selectedAtoms().front(),
                            selectedAction == selectSimilarDirect ? 0 : (selectedAction == selectSimilarPrimary ? 1 : 2));
        auto Z = species_->selectedAtoms().front()->Z();
        Messenger::print("NETA definition is '{}'.\n", neta.definitionString());
        for (auto &i : species_->atoms())
            if (i.Z() == Z && neta.matches(&i))
                species_->selectAtom(&i);
        Messenger::print("Selected {} additional atoms.\n", species_->nSelectedAtoms() - 1);
        speciesRenderable_->recreateSelectionPrimitive();
        emit(atomsChanged());
        postRedisplay();
    }

    // Cancel any current interaction
    cancelInteraction();
}

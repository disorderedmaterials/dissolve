// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "gui/gui.h"
#include "gui/speciesViewer.hui"
#include "neta/neta.h"
#include <QtGui/QMouseEvent>

// Mouse moved
void SpeciesViewer::mouseMoved(int dx, int dy)
{
    // If we are not actually interacting with the view, return now
    if (!interacting())
        return;

    auto refresh = false;
    std::optional<int> currentAtomIndex;

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
                    currentAtomIndex = atomIndexAt(rMouseLast_.x, rMouseLast_.y);

                    // Set the current drawing coordinates in data-space
                    drawCoordinateCurrent_ = currentAtomIndex ? species_->atom(currentAtomIndex.value()).r()
                                                              : view().screenToData(rMouseLast_.x, rMouseLast_.y, 0.0);

                    // Update the interaction Primitive
                    if (clickedAtomIndex_)
                    {
                        if (currentAtomIndex)
                            speciesRenderable_->recreateDrawInteractionPrimitive(&species_->atom(clickedAtomIndex_.value()),
                                                                                 &species_->atom(currentAtomIndex.value()));
                        else
                            speciesRenderable_->recreateDrawInteractionPrimitive(&species_->atom(clickedAtomIndex_.value()),
                                                                                 drawCoordinateCurrent_, drawElement_);
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
                    if (clickedAtomIndex_)
                    {
                        currentAtomIndex = atomIndexAt(rMouseLast_.x, rMouseLast_.y);
                        speciesRenderable_->recreateDeleteInteractionPrimitive(
                            &species_->atom(clickedAtomIndex_.value()),
                            currentAtomIndex ? &species_->atom(currentAtomIndex.value()) : nullptr);
                    }
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

// Toggle visibility of site management menu actions in the context menu
void SpeciesViewer::setSiteActionsVisible(bool visible) { siteActionsVisible_ = visible; }

// Context menu requested
void SpeciesViewer::contextMenuRequested(QPoint pos)
{
    QMenu menu(this);
    menu.setFont(font());

    std::map<QAction *, std::string> actionMap;

    // Reset View
    actionMap[menu.addAction("&Reset View")] = "ResetView";

    // Copy to clipboard
    actionMap[menu.addAction("&Copy to clipboard")] = "CopyToClipboard";

    // Atom selection context menu?
    auto nSelected = species_->selectedAtoms().size();
    if (species_ && nSelected > 0)
    {
        menu.addSection("Current Selection");

        // Atom select submenu
        auto *selectMenu = menu.addMenu("Select similar atoms...");
        selectMenu->setFont(font());
        if (nSelected == 1)
        {
            actionMap[selectMenu->addAction("By direct connectivity")] = "SelectDirect";
            actionMap[selectMenu->addAction("Based on primary neighbours")] = "SelectPrimary";
            actionMap[selectMenu->addAction("Based on primary and secondary neighbours")] = "SelectSecondary";
        }
        else
            selectMenu->setEnabled(false);

        auto *createSiteMenu = menu.addMenu("Create site from...");
        createSiteMenu->setFont(font());
        actionMap[createSiteMenu->addAction("Atoms")] = "CreateStatic";
        actionMap[createSiteMenu->addAction("Elements")] = "CreateDynamicElements";
        actionMap[createSiteMenu->addAction("Atom Types")] = "CreateDynamicAtomTypes";
        actionMap[createSiteMenu->addAction("Empty Fragment String")] = "CreateFragment";
        createSiteMenu->setEnabled(nSelected > 0);
        createSiteMenu->menuAction()->setVisible(siteActionsVisible_);

        auto *modifySiteMenu = menu.addMenu("Modify current site...");
        modifySiteMenu->setFont(font());
        if (site_ != nullptr)
        {
            if (site_->type() == SpeciesSite::SiteType::Static)
            {
                actionMap[modifySiteMenu->addAction("Set Origin Atoms")] = "SetOriginStatic";
                actionMap[modifySiteMenu->addAction("Set X-Axis Atoms")] = "SetXStatic";
                actionMap[modifySiteMenu->addAction("Set Y-Axis Atoms")] = "SetYStatic";
            }
            else if (site_->type() == SpeciesSite::SiteType::Dynamic)
            {
                actionMap[modifySiteMenu->addAction("Set Elements")] = "SetDynamicElements";
                actionMap[modifySiteMenu->addAction("Set Atom Types")] = "SetDynamicAtomTypes";
            }
            else if (site_->type() == SpeciesSite::SiteType::Fragment)
                modifySiteMenu->setEnabled(false);
        }
        modifySiteMenu->setEnabled(sitesVisible_);
        modifySiteMenu->menuAction()->setVisible(siteActionsVisible_);

        // Set menu (only if DissolveWindow is set)
        if (dissolveWindow_)
        {
            auto *setMenu = menu.addMenu("Set...");
            setMenu->setFont(font());
            auto *setAtomTypeAction = setMenu->addAction("Atom type...");
            setAtomTypeAction->setEnabled(species_->isSelectionSingleElement());
            connect(setAtomTypeAction, SIGNAL(triggered(bool)), dissolveWindow_.value(),
                    SLOT(on_SpeciesSetAtomTypesInSelectionAction_triggered(bool)));
            connect(setMenu->addAction("Charges..."), SIGNAL(triggered(bool)), dissolveWindow_.value(),
                    SLOT(on_SpeciesSetChargesInSelectionAction_triggered(bool)));
        }
    }

    // Execute the menu
    auto *selectedAction = menu.exec(mapToGlobal(pos));
    if (selectedAction == nullptr)
    {
        cancelInteraction();
        return;
    }

    // Act on the action!
    if (actionMap[selectedAction] == "ResetView")
        view_.resetViewMatrix();
    else if (actionMap[selectedAction] == "CopyToClipboard")
        copyViewToClipboard(true);
    else if (actionMap[selectedAction] == "CreateStatic")
    {
        auto *site =
            species_->addSite(EmpiricalFormula::formula(species_->selectedAtoms(), [](const auto &i) { return i->Z(); }));
        site->setStaticOriginAtoms(species_->selectedAtoms());
        setSite(site);
        postRedisplay();
        Q_EMIT(siteCreatedAndShown());
    }
    else if (actionMap[selectedAction] == "SetOriginStatic")
    {
        site_->setStaticOriginAtoms(species_->selectedAtoms());
        postRedisplay();
        Q_EMIT(sitesChanged());
    }
    else if (actionMap[selectedAction] == "SetXStatic")
    {
        site_->setStaticXAxisAtoms(species_->selectedAtoms());
        postRedisplay();
        Q_EMIT(sitesChanged());
    }
    else if (actionMap[selectedAction] == "SetYStatic")
    {
        site_->setStaticYAxisAtoms(species_->selectedAtoms());
        postRedisplay();
        Q_EMIT(sitesChanged());
    }
    else if (actionMap[selectedAction] == "CreateDynamicElements")
    {
        auto *site =
            species_->addSite(EmpiricalFormula::formula(species_->selectedAtoms(), [](const auto &i) { return i->Z(); }));
        site->setType(SpeciesSite::SiteType::Dynamic);
        std::vector<Elements::Element> elements;
        for (const auto &i : species_->selectedAtoms())
            elements.push_back(i->Z());
        site->setDynamicElements(elements);
        setSite(site);
        postRedisplay();
        Q_EMIT(siteCreatedAndShown());
    }
    else if (actionMap[selectedAction] == "CreateDynamicAtomTypes")
    {
        auto *site =
            species_->addSite(joinStrings(species_->selectedAtoms(), "_", [](const auto &i) { return i->atomType()->name(); }));
        site->setType(SpeciesSite::SiteType::Dynamic);
        std::vector<std::shared_ptr<AtomType>> atomTypes;
        for (const auto &i : species_->selectedAtoms())
            atomTypes.push_back(i->atomType());
        site->setDynamicAtomTypes(atomTypes);
        setSite(site);
        postRedisplay();
        Q_EMIT(siteCreatedAndShown());
    }
    else if (actionMap[selectedAction] == "SetDynamicElements")
    {
        std::vector<Elements::Element> elements;
        for (const auto &i : species_->selectedAtoms())
            elements.push_back(i->Z());
        site_->setDynamicElements(elements);
        postRedisplay();
        Q_EMIT(sitesChanged());
    }
    else if (actionMap[selectedAction] == "SetDynamicAtomTypes")
    {
        std::vector<std::shared_ptr<AtomType>> atomTypes;
        for (const auto &i : species_->selectedAtoms())
            atomTypes.push_back(i->atomType());
        site_->setDynamicAtomTypes(atomTypes);
        postRedisplay();
        Q_EMIT(sitesChanged());
    }
    else if (actionMap[selectedAction] == "CreateFragment")
    {
        auto *site = species_->addSite("New Site");
        site->setType(SpeciesSite::SiteType::Fragment);
        setSite(site);
        postRedisplay();
        Q_EMIT(siteCreatedAndShown());
    }
    else if (DissolveSys::startsWith(actionMap[selectedAction], "Select"))
    {
        // Create a NETA description from the current atom
        NETADefinition neta(species_->selectedAtoms().front(), actionMap[selectedAction] == "SelectDirect"
                                                                   ? 0
                                                                   : (actionMap[selectedAction] == "SelectPrimary" ? 1 : 2));
        auto Z = species_->selectedAtoms().front()->Z();
        Messenger::print("NETA definition is '{}'.\n", neta.definitionString());
        for (auto &i : species_->atoms())
            if (i.Z() == Z && neta.matches(&i))
                species_->selectAtom(i.index());
        Messenger::print("Selected {} additional atoms.\n", species_->selectedAtoms().size() - 1);
        speciesRenderable_->recreateSelectionPrimitive();
        Q_EMIT(atomsChanged());
        postRedisplay();
    }

    // Cancel any current interaction
    cancelInteraction();
}

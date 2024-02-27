// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/speciesEditor.h"
#include "classes/empiricalFormula.h"
#include "classes/species.h"
#include "data/ff/library.h"
#include "gui/widgets/elementSelector.h"
#include "main/dissolve.h"
#include "modules/geomOpt/geomOpt.h"
#include <QButtonGroup>
#include <QMessageBox>

SpeciesEditor::SpeciesEditor(QWidget *parent) : QWidget(parent)
{
    dissolve_ = nullptr;

    // Set up our UI
    ui_.setupUi(this);

    // Create a button group for the interaction modes
    auto *group = new QButtonGroup;
    group->addButton(ui_.InteractionViewButton);
    group->addButton(ui_.InteractionDrawButton);
    group->addButton(ui_.InteractionDeleteButton);
    speciesViewer()->setInteractionMode(SpeciesViewer::InteractionMode::Draw);

    // Connect signals / slots
    connect(ui_.SpeciesView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.SpeciesView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
    connect(ui_.SpeciesView, SIGNAL(atomsChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.SpeciesView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));

    // Make sure our controls are consistent with the underlying viewer / data
    updateToolbar();
    updateStatusBar();
}

// Set main Dissolve pointer
void SpeciesEditor::setDissolve(Dissolve *dissolve) { dissolve_ = dissolve; }

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void SpeciesEditor::notifyStyleModified() { Q_EMIT(styleModified()); }

// Notify that the displayed data in the underlying viewer has changed
void SpeciesEditor::notifyDataModified() { Q_EMIT(dataModified()); }

// Post redisplay in the underlying view
void SpeciesEditor::postRedisplay() { ui_.SpeciesView->postRedisplay(); }

// Update toolbar to reflect current viewer state
void SpeciesEditor::updateToolbar()
{
    // Set current interaction mode
    switch (speciesViewer()->interactionMode())
    {
        case (SpeciesViewer::InteractionMode::Select):
        case (SpeciesViewer::InteractionMode::SelectArea):
            ui_.InteractionViewButton->setChecked(true);
            break;
        case (SpeciesViewer::InteractionMode::Draw):
            ui_.InteractionDrawButton->setChecked(true);
            break;
        case (SpeciesViewer::InteractionMode::Delete):
            ui_.InteractionDeleteButton->setChecked(true);
            break;
        default:
            break;
    }

    // Set drawing element symbol
    ui_.InteractionDrawElementButton->setText(
        QString::fromStdString(std::string(Elements::symbol(speciesViewer()->drawElement()))));

    // Set checkable buttons
    ui_.ViewAxesVisibleButton->setChecked(speciesViewer()->axesVisible());
    ui_.ViewSpheresButton->setChecked(speciesViewer()->renderableDrawStyle() != RenderableSpecies::LinesStyle);
}

// Update status bar
void SpeciesEditor::updateStatusBar()
{
    // Get displayed Species
    const auto *sp = speciesViewer()->species();
    if (!sp)
        return;

    // Set interaction mode text
    ui_.ModeLabel->setText(speciesViewer()->interactionModeText());

    // Set / update empirical formula for the Species and its current atom selection
    auto selection = sp->selectedAtoms();
    ui_.FormulaLabel->setText(QString::fromStdString(EmpiricalFormula::formula(
        sp->atoms(), [](const auto &i) { return i.Z(); }, true)));
    ui_.SelectionLabel->setText(selection.empty() ? "--"
                                                  : QString::fromStdString(EmpiricalFormula::formula(
                                                        selection, [](const auto &i) { return i->Z(); }, true)));
}

/*
 * Species Viewer
 */

// Set target Species, updating widget as necessary
void SpeciesEditor::setSpecies(Species *sp)
{
    ui_.SpeciesView->setSpecies(sp);

    updateToolbar();
    updateStatusBar();
}

// Return contained SpeciesViewer
SpeciesViewer *SpeciesEditor::speciesViewer() { return ui_.SpeciesView; }

/*
 * Toolbar
 */

void SpeciesEditor::on_InteractionViewButton_clicked(bool checked)
{
    if (checked)
        speciesViewer()->setInteractionMode(SpeciesViewer::InteractionMode::Select);
}

void SpeciesEditor::on_InteractionDrawButton_clicked(bool checked)
{
    if (checked)
        speciesViewer()->setInteractionMode(SpeciesViewer::InteractionMode::Draw);
}

void SpeciesEditor::on_InteractionDrawElementButton_clicked(bool checked)
{
    // Select a new element for drawing
    bool ok;
    auto newElement = ElementSelector::getElement(this, "Choose Element", "Select element to use for drawn atoms",
                                                  speciesViewer()->drawElement(), &ok);
    if (!ok)
        return;

    speciesViewer()->setDrawElement(newElement);

    updateToolbar();
}

void SpeciesEditor::on_InteractionDeleteButton_clicked(bool checked)
{
    if (checked)
        speciesViewer()->setInteractionMode(SpeciesViewer::InteractionMode::Delete);
}

void SpeciesEditor::on_ViewResetButton_clicked(bool checked)
{
    speciesViewer()->view().showAllData();
    speciesViewer()->view().resetViewMatrix();

    speciesViewer()->postRedisplay();
}

void SpeciesEditor::on_ViewSpheresButton_clicked(bool checked)
{
    speciesViewer()->setRenderableDrawStyle(checked ? RenderableSpecies::SpheresStyle : RenderableSpecies::LinesStyle);

    speciesViewer()->notifyDataModified();

    speciesViewer()->postRedisplay();
}

void SpeciesEditor::on_ViewAxesVisibleButton_clicked(bool checked)
{
    speciesViewer()->setAxesVisible(checked);

    speciesViewer()->postRedisplay();
}

void SpeciesEditor::on_ViewCopyToClipboardButton_clicked(bool checked) { speciesViewer()->copyViewToClipboard(checked); }

// Tools
void SpeciesEditor::on_ToolsCalculateBondingButton_clicked(bool checked)
{
    // Get displayed Species
    auto *sp = speciesViewer()->species();
    if (!sp)
        return;

    // Calculate missing bonds
    sp->addMissingBonds();

    // Signal that the data shown has been modified
    speciesViewer()->postRedisplay();
    speciesViewer()->notifyDataModified();
}

void SpeciesEditor::on_ToolsMinimiseButton_clicked(bool checked)
{
    // Get displayed Species
    auto *sp = speciesViewer()->species();
    if (!sp)
        return;

    // Get the pointer to the UFF, and create some temporary objects
    auto uff = ForcefieldLibrary::forcefield("UFF");
    if (!uff)
        throw(std::runtime_error("Couldn't locate UFF to perform minimisation.\n"));
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Clear any old forcefield terms from the species, and recreate higher-order intramolecular terms
    sp->clearForcefieldTerms();
    sp->updateIntramolecularTerms();

    // Apply new forcefield terms
    auto assignErrs = uff->assignAtomTypes(sp, coreData, Forcefield::TypeAll, false);
    if (!assignErrs.empty())
    {
        QMessageBox::critical(
            this, "Error",
            QString("No matching atom types for indices ") +
                QString::fromStdString(joinStrings(assignErrs, ", ", [](const auto &i) { return std::to_string(i); })));
        return;
    }
    if (!uff->assignIntramolecular(sp))
    {
        QMessageBox::critical(this, "Error", "Failed to apply UFF to the species - molecule clean-up is not possible.");
        return;
    }

    // Check that the Species set up is valid
    if (!sp->checkSetUp())
        return;

    // Set up interatomic potentials
    if (!dissolve.regeneratePairPotentials())
    {
        Messenger::error("Couldn't set up interatomic pair potentials - no optimisation performed!\n");
        return;
    }

    // Apply a small randomisation to the Species so we can overcome edge cases in initial geometries (e.g. all atoms in one
    // plane)
    sp->randomiseCoordinates(0.0001);

    // Do the optimisation
    GeometryOptimisationModule optimiser;
    optimiser.optimiseSpecies(dissolve.potentialMap(), dissolve.worldPool(), sp);

    // Centre the Species back at the origin
    sp->centreAtOrigin();

    // Clear the forcefield terms from the species
    sp->clearForcefieldTerms();

    // Need to update the SpeciesViewer, and signal that the data shown has been modified
    speciesViewer()->view().showAllData();
    speciesViewer()->postRedisplay();
    speciesViewer()->notifyDataModified();
}

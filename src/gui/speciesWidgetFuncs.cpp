// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "classes/species.h"
#include "gui/speciesWidget.h"
#include "gui/widgets/elementSelector.hui"
#include "main/dissolve.h"
#include "modules/geomOpt/geomOpt.h"
#include <QButtonGroup>

SpeciesWidget::SpeciesWidget(QWidget *parent) : QWidget(parent)
{
    dissolve_ = nullptr;

    // Set up our UI
    ui_.setupUi(this);

    // Connect signals / slots
    connect(ui_.SpeciesView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.SpeciesView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
    connect(ui_.SpeciesView, SIGNAL(atomsChanged()), this, SLOT(updateStatusBar()));

    // Make sure our controls are consistent with the underlying viewer / data
    updateToolbar();
    updateStatusBar();
}

// Set main Dissolve pointer
void SpeciesWidget::setDissolve(Dissolve *dissolve) { dissolve_ = dissolve; }

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void SpeciesWidget::notifyStyleModified() { emit(styleModified()); }

// Notify that the displayed data in the underlying viewer has changed
void SpeciesWidget::notifyDataModified() { emit(dataModified()); }

// Post redisplay in the underlying view
void SpeciesWidget::postRedisplay() { ui_.SpeciesView->postRedisplay(); }

// Update toolbar to reflect current viewer state
void SpeciesWidget::updateToolbar()
{
    // Set checkable buttons
    ui_.ViewAxesVisibleButton->setChecked(speciesViewer()->axesVisible());
    ui_.ViewSpheresButton->setChecked(speciesViewer()->renderableDrawStyle() != RenderableSpecies::LinesStyle);
}

// Update status bar
void SpeciesWidget::updateStatusBar()
{
    // Get displayed Species
    const auto *sp = speciesViewer()->species();

    // Set interaction mode text
    ui_.ModeLabel->setText(speciesViewer()->interactionModeText());

    // Set / update empirical formula for the Species and its current atom selection
    if (sp)
    {
        auto selection = sp->selectedAtoms();
        ui_.FormulaLabel->setText(QString::fromStdString(EmpiricalFormula::formula(
            sp->atoms(), [](const auto &i) { return i.Z(); }, true)));
        ui_.SelectionLabel->setText(!selection.empty() ? QString::fromStdString(EmpiricalFormula::formula(
                                                             selection, [](const auto &i) { return i->Z(); }, true))
                                                       : "--");
    }
}

/*
 * Species Viewer
 */

// Set target Species, updating widget as necessary
void SpeciesWidget::setSpecies(Species *sp)
{
    ui_.SpeciesView->setSpecies(sp);

    updateToolbar();
    updateStatusBar();
}

// Set target SpeciesSite, updating widget as necessary
void SpeciesWidget::setSite(SpeciesSite *site)
{
    ui_.SpeciesView->setSite(site);

    updateToolbar();
    updateStatusBar();
}

// Return contained SpeciesViewer
SpeciesViewer *SpeciesWidget::speciesViewer() { return ui_.SpeciesView; }

/*
 * Toolbar
 */

void SpeciesWidget::on_ViewResetButton_clicked(bool checked)
{
    speciesViewer()->view().showAllData();
    speciesViewer()->view().resetViewMatrix();

    speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewSpheresButton_clicked(bool checked)
{
    speciesViewer()->setRenderableDrawStyle(checked ? RenderableSpecies::SpheresStyle : RenderableSpecies::LinesStyle);

    speciesViewer()->notifyDataModified();

    speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewAxesVisibleButton_clicked(bool checked)
{
    speciesViewer()->setAxesVisible(checked);

    speciesViewer()->postRedisplay();
}

void SpeciesWidget::on_ViewCopyToClipboardButton_clicked(bool checked) { speciesViewer()->copyViewToClipboard(checked); }

void SpeciesWidget::on_ToolsMinimiseButton_clicked(bool checked)
{
    // Get displayed Species
    auto *sp = speciesViewer()->species();
    if (!sp)
        return;

    // Apply forcefield terms now?
    if (sp->forcefield())
        sp->applyForcefieldTerms(dissolve_->coreData());

    // Check that the Species set up is valid
    if (!sp->checkSetUp())
        return;

    // Set up interatomic potentials
    if (!dissolve_->regeneratePairPotentials())
    {
        Messenger::error("Couldn't set up interatomic pair potentials - no optimisation performed!\n");
        return;
    }

    // Apply a small randomisation to the Species so we can overcome edge cases in initial geometries (e.g. all atoms in one
    // plane)
    sp->randomiseCoordinates(0.0001);

    // Do the optimisation
    GeometryOptimisationModule optimiser;
    optimiser.optimiseSpecies(dissolve_->potentialMap(), dissolve_->worldPool(), sp);

    // Centre the Species back at the origin
    sp->centreAtOrigin();

    // Need to update the SpeciesViewer, and signal that the data shown has been modified
    speciesViewer()->view().showAllData();
    speciesViewer()->postRedisplay();
    speciesViewer()->notifyDataModified();
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/addconfigurationdialog.h"
#include "templates/algorithms.h"
#include <QInputDialog>
#include <QMessageBox>

AddConfigurationDialog::AddConfigurationDialog(QWidget *parent, Dissolve &dissolve) : WizardDialog(parent), dissolve_(dissolve)
{
    ui_.setupUi(this);

    // Set up sub-widgets
    ui_.TargetSpeciesWidget->setSpecies(dissolve_.species());

    // Register pages with the wizard
    registerPage(AddConfigurationDialog::SelectForcefieldPage, "Target Species", AddConfigurationDialog::AnotherPage);
    initialise(this, ui_.MainStack, AddConfigurationDialog::SelectForcefieldPage);
}

/*
 * Controls
 */

// Return whether progression to the next page from the current page is allowed
bool AddConfigurationDialog::progressionAllowed(int index) const
{
    if (index == AddConfigurationDialog::SelectForcefieldPage)
    {
        // Must have at least one species, and not more than one periodic species
        if (ui_.TargetSpeciesWidget->nSelected() == 0 ||
            std::count_if(ui_.TargetSpeciesWidget->currentSpecies().begin(), ui_.TargetSpeciesWidget->currentSpecies().end(),
                          [](const auto *sp) { return sp->box()->type() != Box::BoxType::NonPeriodic; }) > 1)
            return false;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool AddConfigurationDialog::prepareForNextPage(int currentIndex) { return true; }

// Determine next page for the current page, based on current data
std::optional<int> AddConfigurationDialog::determineNextPage(int currentIndex) { return std::nullopt; }

// Perform any necessary actions before moving to the previous page
bool AddConfigurationDialog::prepareForPreviousPage(int currentIndex) { return true; }

// Perform any final actions before the wizard is closed
void AddConfigurationDialog::finalise() {}

/*
 * Select Forcefield Page
 */

void AddConfigurationDialog::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid) {}

void AddConfigurationDialog::on_ForcefieldWidget_forcefieldDoubleClicked() {}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/importspeciesdialog.h"

ImportSpeciesDialog::ImportSpeciesDialog(QWidget *parent, const Dissolve &mainDissolveInstance)
{
    ui_.setupUi(this);

    // Set up and connect the AddSpeciesWidget
    ui_.ImportSpeciesWidget->setMainDissolveReference(&mainDissolveInstance);
    ui_.ImportSpeciesWidget->setCloseButtonAvailable(false);
    connect(ui_.ImportSpeciesWidget, SIGNAL(finished()), this, SLOT(accept()));
    connect(ui_.ImportSpeciesWidget, SIGNAL(canceled()), this, SLOT(reject()));
}

ImportSpeciesDialog::~ImportSpeciesDialog() {}

// Reset, ready for adding a new Species
void ImportSpeciesDialog::reset() { ui_.ImportSpeciesWidget->reset(); }

// Copy imported Species over to the specified Dissolve object
Species *ImportSpeciesDialog::importSpecies(Dissolve &dissolve) { return ui_.ImportSpeciesWidget->importSpecies(dissolve); }

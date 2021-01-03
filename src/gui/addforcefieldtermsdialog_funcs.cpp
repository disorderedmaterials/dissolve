// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/addforcefieldtermsdialog.h"

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent, const Dissolve &mainDissolveInstance)
{
    ui_.setupUi(this);

    // Set up and connect the AddForcefieldTermsWidget
    ui_.AddForcefieldTermsWidget->setMainDissolveReference(&mainDissolveInstance);
    ui_.AddForcefieldTermsWidget->setCloseButtonAvailable(false);
    connect(ui_.AddForcefieldTermsWidget, SIGNAL(finished()), this, SLOT(accept()));
    connect(ui_.AddForcefieldTermsWidget, SIGNAL(canceled()), this, SLOT(reject()));
}

AddForcefieldTermsDialog::~AddForcefieldTermsDialog() {}

// Reset, ready for adding a new Configuration
void AddForcefieldTermsDialog::reset() { ui_.AddForcefieldTermsWidget->reset(); }

// Set target Species that we are acquiring forcefield terms for
void AddForcefieldTermsDialog::setTargetSpecies(Species *sp) { ui_.AddForcefieldTermsWidget->setTargetSpecies(sp); }

// Apply Forcefield terms to the targetted Species within the specified Dissolve object
bool AddForcefieldTermsDialog::applyForcefieldTerms(Dissolve &dissolve)
{
    return ui_.AddForcefieldTermsWidget->applyForcefieldTerms(dissolve);
}

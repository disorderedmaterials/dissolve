// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/speciesCellDialog.h"

SpeciesCellDialog::SpeciesCellDialog(QWidget *parent, Species *sp)
{
    // Setup dialog
    ui_.setupUi(this);
    ui_.BoxASpin->setValue(20.0);
    ui_.BoxBSpin->setValue(20.0);
    ui_.BoxCSpin->setValue(20.0);
    ui_.BoxAlphaSpin->setValue(90.0);
    ui_.BoxBetaSpin->setValue(90.0);
    ui_.BoxGammaSpin->setValue(90.0);

    sp_ = sp;
}

void SpeciesCellDialog::on_buttonBox_accepted()
{
    // Generate a box for the species
    sp_->createBox({ui_.BoxASpin->value(), ui_.BoxBSpin->value(), ui_.BoxCSpin->value()},
                   {ui_.BoxAlphaSpin->value(), ui_.BoxBetaSpin->value(), ui_.BoxGammaSpin->value()});
    Messenger::print("New unit cell for species generated");
    accept();
}

void SpeciesCellDialog::on_buttonBox_rejected() { reject(); }

// Run the dialog, returning whether it was accepted
bool SpeciesCellDialog::createUnitCell() { return (exec() == QDialog::Accepted); }
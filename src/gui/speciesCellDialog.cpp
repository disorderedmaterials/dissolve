// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/speciesCellDialog.h"

SpeciesCellDialog::SpeciesCellDialog(QWidget *parent, Species *sp)
{
    // Setup dialog
    ui_.setupUi(this);
    sp_ = sp;
}

void SpeciesCellDialog::on_buttonBox_accepted()
{
    // Generate a box for the species
    Vec3<double> axisLengths{ui_.BoxASpin->value(), ui_.BoxBSpin->value(), ui_.BoxCSpin->value()};
    Vec3<double> axisAngles{ui_.BoxAlphaSpin->value(), ui_.BoxBetaSpin->value(), ui_.BoxGammaSpin->value()};
    sp_->createBox(axisLengths, axisAngles);
    Messenger::print("New unit cell for species generated");
    accept();
}

void SpeciesCellDialog::on_buttonBox_rejected() { reject(); }
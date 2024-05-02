// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/editSpeciesDialog.h"

EditSpeciesDialog::EditSpeciesDialog(QWidget *parent, Species *sp)
{
    ui_.setupUi(this);

    species_ = sp;

    connect(ui_.EditSpeciesWidget, SIGNAL(dataModified()), this, SLOT(dataModified()));

    ui_.EditSpeciesWidget->setSpecies(species_);

    ui_.OKButton->setEnabled(species_ && species_->nAtoms() > 0);
}

EditSpeciesDialog::~EditSpeciesDialog() {}

void EditSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }

void EditSpeciesDialog::on_OKButton_clicked(bool checked) { accept(); }

void EditSpeciesDialog::dataModified() { ui_.OKButton->setEnabled(species_->nAtoms() > 0); }

// Run the dialog, returning whether it was accepted
bool EditSpeciesDialog::editSpecies() { return (exec() == QDialog::Accepted); }

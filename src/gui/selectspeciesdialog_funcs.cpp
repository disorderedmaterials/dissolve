// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/ff.h"
#include "gui/selectspeciesdialog.h"
#include "gui/selectspecieswidget.h"
#include "templates/variantpointer.h"
#include <QRegExp>

SelectSpeciesDialog::SelectSpeciesDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle)
{
    ui_.setupUi(this);

    setWindowTitle(dialogTitle);

    ui_.SpeciesWidget->setCoreData(&coreData);
}

SelectSpeciesDialog::~SelectSpeciesDialog() {}

void SelectSpeciesDialog::on_SpeciesWidget_speciesSelectionChanged(bool isValid) { ui_.SelectButton->setEnabled(isValid); }

void SelectSpeciesDialog::on_SpeciesWidget_speciesDoubleClicked()
{
    // Check current selection size for validity
    if (ui_.SpeciesWidget->currentSpecies().nItems() != 1)
        return;

    accept();
}

void SelectSpeciesDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning a single selected Species
Species *SelectSpeciesDialog::selectSpecies()
{
    ui_.SpeciesWidget->reset(1, 1);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.SpeciesWidget->currentSpecies().firstItem();
    else
        return nullptr;
}

// Run the dialog, returning a list of selected Species
RefList<Species> SelectSpeciesDialog::selectSpecies(int minSpecies, int maxSpecies)
{
    ui_.SpeciesWidget->reset(minSpecies, maxSpecies);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.SpeciesWidget->currentSpecies();
    else
        return RefList<Species>();
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectSpeciesDialog.h"
#include "classes/coreData.h"
#include "data/ff/ff.h"
#include "gui/selectSpeciesWidget.h"

SelectSpeciesDialog::SelectSpeciesDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle)
{
    ui_.setupUi(this);

    setWindowTitle(dialogTitle);

    ui_.SpeciesWidget->setSpecies(coreData.species());
}

void SelectSpeciesDialog::on_SpeciesWidget_speciesSelectionChanged(bool isValid) { ui_.SelectButton->setEnabled(isValid); }

void SelectSpeciesDialog::on_SpeciesWidget_speciesDoubleClicked()
{
    // Check current selection size for validity
    if (ui_.SpeciesWidget->selection().size() != 1)
        return;

    accept();
}

void SelectSpeciesDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning a single selected Species
const Species *SelectSpeciesDialog::selectSingleSpecies(const Flags<SpeciesFilterProxy::FilterFlags> &flags)
{
    ui_.SpeciesWidget->reset(1);
    ui_.SpeciesWidget->setFilterProxyFlags(flags);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.SpeciesWidget->nSelected() == 0 ? nullptr : ui_.SpeciesWidget->selection().front();
    else
        return nullptr;
}

// Run the dialog, returning a list of selected Species
std::vector<const Species *> SelectSpeciesDialog::selectSpecies(const Flags<SpeciesFilterProxy::FilterFlags> &flags,
                                                                int minSpecies, std::optional<int> maxSpecies)
{
    ui_.SpeciesWidget->reset(minSpecies, maxSpecies);
    ui_.SpeciesWidget->setFilterProxyFlags(flags);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.SpeciesWidget->selection();
    else
        return {};
}

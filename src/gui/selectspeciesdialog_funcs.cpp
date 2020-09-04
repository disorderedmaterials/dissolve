/*
    *** Select Species Dialog
    *** src/gui/selectspeciesdialog_funcs.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "data/ff.h"
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

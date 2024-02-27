// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectAtomTypeDialog.h"
#include "classes/coreData.h"
#include "gui/selectAtomTypeWidget.h"

SelectAtomTypeDialog::SelectAtomTypeDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle)
{
    ui_.setupUi(this);

    setWindowTitle(dialogTitle);

    ui_.AtomTypeWidget->setAtomTypes(coreData.atomTypes(), coreData);
}

void SelectAtomTypeDialog::on_AtomTypeWidget_atomTypeSelectionChanged(bool isValid) { ui_.SelectButton->setEnabled(isValid); }

void SelectAtomTypeDialog::on_AtomTypeWidget_atomTypeDoubleClicked()
{
    // Check current selection size for validity
    if (ui_.AtomTypeWidget->currentAtomType() == nullptr)
        return;

    accept();
}

void SelectAtomTypeDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectAtomTypeDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning a single selected AtomType
const std::shared_ptr<AtomType> SelectAtomTypeDialog::selectAtomType(std::optional<Elements::Element> filterElement)
{
    if (filterElement.has_value())
        ui_.AtomTypeWidget->setFilterElement(filterElement.value());
    else
        ui_.AtomTypeWidget->clearFilterElement();
    ui_.AtomTypeWidget->reset();

    show();

    if (exec() == QDialog::Accepted)
        return ui_.AtomTypeWidget->currentAtomType();
    else
        return nullptr;
}

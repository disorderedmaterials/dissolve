// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "gui/getSpeciesNameDialog.h"

GetSpeciesNameDialog::GetSpeciesNameDialog(QWidget *parent, const CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);
}

GetSpeciesNameDialog::~GetSpeciesNameDialog() {}

// Run the dialog
bool GetSpeciesNameDialog::get(const Species *species, const QString &currentName)
{
    species_ = species;

    // Set the current text
    ui_.NameEdit->setText(currentName);

    show();

    return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetSpeciesNameDialog::newName() const { return ui_.NameEdit->text(); }

/*
 * Slots
 */

// Species name edited
void GetSpeciesNameDialog::on_NameEdit_textChanged(const QString text)
{
    // Make sure the name is valid
    auto nameValid = true;
    if (text.isEmpty())
        nameValid = false;
    else
    {
        for (const auto &sp : coreData_.species())
        {
            if (species_ == sp.get())
                continue;

            if (DissolveSys::sameString(sp->name(), qPrintable(text)))
            {
                nameValid = false;
                break;
            }
        }
    }

    // Update indicator
    ui_.NameIndicator->setOK(nameValid);

    // Update dialog buttons
    ui_.OKButton->setEnabled(nameValid);
}

void GetSpeciesNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetSpeciesNameDialog::on_OKButton_clicked(bool checked) { accept(); }

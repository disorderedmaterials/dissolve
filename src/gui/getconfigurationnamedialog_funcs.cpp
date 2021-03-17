// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/getconfigurationnamedialog.h"

GetConfigurationNameDialog::GetConfigurationNameDialog(QWidget *parent, const CoreData &coreData) : coreData_(coreData)
{
    ui_.setupUi(this);
}

GetConfigurationNameDialog::~GetConfigurationNameDialog() {}

// Run the dialog
bool GetConfigurationNameDialog::get(const Configuration *currentConfiguration, const QString &currentName)
{
    configuration_ = currentConfiguration;

    // Set the current text
    ui_.NameEdit->setText(currentName);

    show();

    return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetConfigurationNameDialog::newName() const { return ui_.NameEdit->text(); }

/*
 * Slots
 */

// Configuration name edited
void GetConfigurationNameDialog::on_NameEdit_textChanged(const QString text)
{
    // Make sure the name is valid
    auto nameValid = true;
    if (text.isEmpty())
        nameValid = false;
    else
    {
        ListIterator<Configuration> configIterator(coreData_.configurations());
        while (Configuration *cfg = configIterator.iterate())
        {
            if (configuration_ == cfg)
                continue;

            if (DissolveSys::sameString(cfg->name(), qPrintable(text)))
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

void GetConfigurationNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetConfigurationNameDialog::on_OKButton_clicked(bool checked) { accept(); }

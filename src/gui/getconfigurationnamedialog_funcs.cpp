/*
    *** Get Configuration Name Dialog
    *** src/gui/getconfigurationnamedialog_funcs.cpp
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
        ListIterator<Configuration> configIterator(coreData_.constConfigurations());
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

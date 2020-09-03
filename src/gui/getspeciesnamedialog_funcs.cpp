/*
    *** Get Species Name Dialog
    *** src/gui/getspeciesnamedialog_funcs.cpp
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
#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/getspeciesnamedialog.h"

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
        ListIterator<Species> speciesIterator(coreData_.constSpecies());
        while (Species *sp = speciesIterator.iterate())
        {
            if (species_ == sp)
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

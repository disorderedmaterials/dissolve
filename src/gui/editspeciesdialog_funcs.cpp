/*
	*** Edit Species Dialog
	*** src/gui/editspeciesdialog_funcs.cpp
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

#include "gui/editspeciesdialog.h"

// Constructor
EditSpeciesDialog::EditSpeciesDialog(QWidget* parent, Species* sp)
{
	ui_.setupUi(this);

	species_ = sp;

	connect(ui_.EditSpeciesWidget, SIGNAL(dataModified()), this, SLOT(dataModified()));

	ui_.EditSpeciesWidget->setSpecies(species_);

	ui_.OKButton->setEnabled(species_ && species_->nAtoms() > 0);
}

// Destructor
EditSpeciesDialog::~EditSpeciesDialog()
{
}

void EditSpeciesDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void EditSpeciesDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditSpeciesDialog::dataModified()
{
	ui_.OKButton->setEnabled(species_->nAtoms() > 0);
}

// Run the dialog, returning whether it was accepted
bool EditSpeciesDialog::editSpecies()
{
	exec();
}

/*
	*** Add Species Dialog
	*** src/gui/addspeciesdialog_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/addspeciesdialog.h"

// Constructor
AddSpeciesDialog::AddSpeciesDialog(QWidget* parent, const Dissolve& mainDissolveInstance)
{
	ui_.setupUi(this);

	// Set up and connect the AddSpeciesWidget
	ui_.AddSpeciesWidget->setMainDissolveReference(&mainDissolveInstance);
	ui_.AddSpeciesWidget->setCloseButtonAvailable(false);
	connect(ui_.AddSpeciesWidget, SIGNAL(finished()), this, SLOT(accept()));
}

// Destructor
AddSpeciesDialog::~AddSpeciesDialog()
{
}

// Reset, ready for adding a new Species
void AddSpeciesDialog::reset()
{
	ui_.AddSpeciesWidget->reset();
}

// Copy imported Species over to the specified Dissolve object
Species* AddSpeciesDialog::importSpecies(Dissolve& dissolve)
{
	return ui_.AddSpeciesWidget->importSpecies(dissolve);
}

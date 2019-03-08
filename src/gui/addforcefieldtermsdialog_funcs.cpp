/*
	*** Add Forcefield Terms Dialog
	*** src/gui/addforcefieldtermsdialog_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/addforcefieldtermsdialog.h"

// Constructor
AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget* parent, const Dissolve& mainDissolveInstance)
{
	ui_.setupUi(this);

	// Set up and connect the AddForcefieldTermsWidget
	ui_.AddForcefieldTermsWidget->setMainDissolveReference(&mainDissolveInstance);
	ui_.AddForcefieldTermsWidget->setCloseButtonAvailable(false);
	connect(ui_.AddForcefieldTermsWidget, SIGNAL(finished()), this, SLOT(accept()));
}

// Destructor
AddForcefieldTermsDialog::~AddForcefieldTermsDialog()
{
}

// Reset, ready for adding a new Configuration
void AddForcefieldTermsDialog::reset()
{
	ui_.AddForcefieldTermsWidget->reset();
}

// Apply Forcefield terms to the targetted Species within the specified Dissolve object
bool AddForcefieldTermsDialog::applyForcefieldTerms(Dissolve& dissolve)
{
	return ui_.AddForcefieldTermsWidget->applyForcefieldTerms(dissolve);
}

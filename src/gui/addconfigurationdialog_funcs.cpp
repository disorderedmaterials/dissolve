/*
	*** Add Configuration Dialog
	*** src/gui/addconfigurationdialog_funcs.cpp
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

#include "gui/addconfigurationdialog.h"

// Constructor
AddConfigurationDialog::AddConfigurationDialog(QWidget* parent, const Dissolve& mainDissolveInstance)
{
	ui_.setupUi(this);

	// Set up and connect the AddConfigurationWidget
	ui_.AddConfigurationWidget->setMainDissolveReference(&mainDissolveInstance);
	ui_.AddConfigurationWidget->setCloseButtonAvailable(false);
	connect(ui_.AddConfigurationWidget, SIGNAL(finished()), this, SLOT(accept()));
}

// Destructor
AddConfigurationDialog::~AddConfigurationDialog()
{
}

// Reset, ready for adding a new Configuration
void AddConfigurationDialog::reset()
{
	ui_.AddConfigurationWidget->reset();
}

// Copy imported Configuration over to the specified Dissolve object
Configuration* AddConfigurationDialog::importConfiguration(Dissolve& dissolve)
{
	return ui_.AddConfigurationWidget->importConfiguration(dissolve);
}

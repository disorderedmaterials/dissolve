/*
	*** Add Processing Layer Dialog
	*** src/gui/addprocessinglayerdialog_funcs.cpp
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

#include "gui/addprocessinglayerdialog.h"

// Constructor
AddProcessingLayerDialog::AddProcessingLayerDialog(QWidget* parent, const Dissolve& mainDissolveInstance)
{
	ui_.setupUi(this);

	// Set up and connect the AddProcessingLayerWidget
	ui_.AddProcessingLayerWidget->setMainDissolveReference(&mainDissolveInstance);
	ui_.AddProcessingLayerWidget->setCloseButtonAvailable(false);
	connect(ui_.AddProcessingLayerWidget, SIGNAL(finished()), this, SLOT(accept()));
}

// Destructor
AddProcessingLayerDialog::~AddProcessingLayerDialog()
{
}

// Reset, ready for adding a new Configuration
void AddProcessingLayerDialog::reset()
{
	ui_.AddProcessingLayerWidget->reset();
}

// Move created / imported layer over to the specified Dissolve object
ModuleLayer* AddProcessingLayerDialog::importModuleLayer(Dissolve& dissolve)
{
	return ui_.AddProcessingLayerWidget->importModuleLayer(dissolve);
}

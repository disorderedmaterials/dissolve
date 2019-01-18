/*
	*** Add Processing Layer Dialog
	*** src/gui/addprocessinglayerdialog.h
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

#ifndef DISSOLVE_ADDPROCESSINGLAYERDIALOG_H
#define DISSOLVE_ADDPROCESSINGLAYERDIALOG_H

#include "gui/ui_addprocessinglayerdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Add Processing Layer Dialog
class AddProcessingLayerDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	AddProcessingLayerDialog(QWidget* parent, const Dissolve& mainDissolveInstance);
	// Destructor
	~AddProcessingLayerDialog();

	private:
	// Main form declaration
	Ui::AddProcessingLayerDialog ui_;

	public:
	// Reset, ready for adding a new Configuration
	void reset();
	// Move created / imported layer over to the specified Dissolve object
	ModuleLayer* importModuleLayer(Dissolve& dissolve);
};

#endif

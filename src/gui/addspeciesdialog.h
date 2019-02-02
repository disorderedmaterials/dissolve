/*
	*** Add Species Dialog
	*** src/gui/addspeciesdialog.h
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

#ifndef DISSOLVE_ADDSPECIESDIALOG_H
#define DISSOLVE_ADDSPECIESDIALOG_H

#include "gui/ui_addspeciesdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Add Species Dialog
class AddSpeciesDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	AddSpeciesDialog(QWidget* parent, const Dissolve& mainDissolveInstance);
	// Destructor
	~AddSpeciesDialog();

	private:
	// Main form declaration
	Ui::AddSpeciesDialog ui_;

	public:
	// Reset, ready for adding a new Species
	void reset();
	// Copy imported Species over to the specified Dissolve object
	Species* importSpecies(Dissolve& dissolve);
};

#endif

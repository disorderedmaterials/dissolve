/*
	*** Edit Species Dialog
	*** src/gui/editspeciesdialog.h
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

#ifndef DISSOLVE_DIALOG_EDITSPECIES_H
#define DISSOLVE_DIALOG_EDITSPECIES_H

#include "gui/ui_editspeciesdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Add Forcefield Terms Dialog
class EditSpeciesDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	EditSpeciesDialog(QWidget* parent, Species* sp);
	// Destructor
	~EditSpeciesDialog();

	private:
	// Target Species
	Species* species_;


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::EditSpeciesDialog ui_;

	private slots:
	void on_CancelButton_clicked(bool checked);
	void on_OKButton_clicked(bool checked);
	void dataModified();

	public:
	// Run the dialog, returning whether it was accepted
	bool editSpecies();
};

#endif

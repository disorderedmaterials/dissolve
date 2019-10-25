/*
	*** Select Species Dialog
	*** src/gui/selectspeciesdialog.h
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

#ifndef DISSOLVE_DIALOG_SELECTSPECIES_H
#define DISSOLVE_DIALOG_SELECTSPECIES_H

#include "gui/ui_selectspeciesdialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;
class SelectSpeciesWidget;

// Select Species Dialog
class SelectSpeciesDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectSpeciesDialog(QWidget* parent, const CoreData& coreData, QString dialogTitle);
	// Destructor
	~SelectSpeciesDialog();

	private:
	// Main form declaration
	Ui::SelectSpeciesDialog ui_;

	private slots:
	void on_SpeciesWidget_speciesSelectionChanged(bool isValid);
	void on_SpeciesWidget_speciesDoubleClicked();
	void on_SelectButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Run the dialog, returning a single selected Species
	Species* selectSpecies();
	// Run the dialog, returning a list of selected Species
	RefList<Species> selectSpecies(int minSpecies, int maxSpecies);
};

#endif

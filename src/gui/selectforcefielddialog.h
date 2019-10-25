/*
	*** Select Forcefield Dialog
	*** src/gui/selectforcefielddialog.h
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

#ifndef DISSOLVE_DIALOG_SELECTFORCEFIELD_H
#define DISSOLVE_DIALOG_SELECTFORCEFIELD_H

#include "gui/ui_selectforcefielddialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Forcefield;
class SelectForcefieldWidget;

// Select Forcefield Dialog
class SelectForcefieldDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectForcefieldDialog(QWidget* parent, const List<Forcefield>& forcefields);
	// Destructor
	~SelectForcefieldDialog();

	private:
	// Main form declaration
	Ui::SelectForcefieldDialog ui_;

	private slots:
	void on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid);
	void on_ForcefieldWidget_forcefieldDoubleClicked();
	void on_SelectButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Run the dialog, returning the selected Forcefield
	Forcefield* selectForcefield(Forcefield* currentFF);
};

#endif

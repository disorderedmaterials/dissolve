/*
	*** Add Forcefield Terms Dialog
	*** src/gui/addforcefieldtermsdialog.h
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

#ifndef DISSOLVE_DIALOG_ADDFORCEFIELDTERMS_H
#define DISSOLVE_DIALOG_ADDFORCEFIELDTERMS_H

#include "gui/ui_addforcefieldtermsdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Add Forcefield Terms Dialog
class AddForcefieldTermsDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	AddForcefieldTermsDialog(QWidget* parent, const Dissolve& mainDissolveInstance);
	// Destructor
	~AddForcefieldTermsDialog();

	private:
	// Main form declaration
	Ui::AddForcefieldTermsDialog ui_;

	public:
	// Reset, ready for adding a new Configuration
	void reset();
	// Set target Species that we are acquiring forcefield terms for
	void setTargetSpecies(Species* sp);
	// Apply Forcefield terms to the targetted Species within the specified Dissolve object
	bool applyForcefieldTerms(Dissolve& dissolve);
};

#endif

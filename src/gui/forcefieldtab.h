/*
	*** Forcefield Tab
	*** src/gui/forcefieldtab.h
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_FORCEFIELDTAB_H
#define DUQ_FORCEFIELDTAB_H

#include "gui/ui_forcefieldtab.h"
#include "gui/maintab.h"

// Forward Declarations
class MasterIntra;
class AtomType;
class PairPotential;

// Forcefield Tab
class ForcefieldTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ForcefieldTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title);
	~ForcefieldTab();
	// Main form declaration
	Ui::ForcefieldTab ui;


	/*
	 * Update
	 */
	private:
	// Row update function for BondsTable
	void updateBondsTableRow(int row, MasterIntra* masterBond, bool createItems);
	// Row update function for AnglesTable
	void updateAnglesTableRow(int row, MasterIntra* masterAngle, bool createItems);
	// Row update function for TorsionsTable
	void updateTorsionsTableRow(int row, MasterIntra* masterTorsion, bool createItems);
	// Row update function for AtomTypesTable
	void updateAtomTypesTableRow(int row, AtomType* atomType, bool createItems);
	// Row update function for PairPotentialsTable
	void updatePairPotentialsTableRow(int row, PairPotential* pairPotential, bool createItems);

	protected:
	// Update controls in tab
	void updateControls();
	// Disable sensitive controls within tab, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within tab, ready for main code to run
	void enableSensitiveControls();


	/*
	 * Signals / Slots
	 */
	private:

	private slots:
	// Atom Types
	void on_AtomTypeAddButton_clicked(bool checked);
	void on_AtomTypeRemoveButton_clicked(bool checked);
	void on_AtomTypeLibraryButton_clicked(bool checked);
	void on_AtomTypesTable_itemChanged(QTableWidgetItem* w);
	// Pair Potentials
	void on_PairPotentialRangeSpin_valueChanged(double value);
	void on_PairPotentialDeltaSpin_valueChanged(double value);
	void on_CoulombIncludeCheck_clicked(bool checked);
	void on_ShortRangeTruncationCombo_currentIndexChanged(int index);
	void on_CoulombTruncationCombo_currentIndexChanged(int index);
	void on_RegenerateAllPairPotentialsButton_clicked(bool checked);
	void on_UpdatePairPotentialsButton_clicked(bool checked);
	void on_GenerateMissingPairPotentialsButton_clicked(bool checked);
	void on_PairPotentialsTable_itemChanged(QTableWidgetItem* w);


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
};

#endif

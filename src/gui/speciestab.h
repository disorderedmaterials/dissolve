/*
	*** Species Tab
	*** src/gui/speciestab.h
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

#ifndef DISSOLVE_SPECIESTAB_H
#define DISSOLVE_SPECIESTAB_H

#include "gui/ui_speciestab.h"
#include "gui/maintab.h"

// Forward Declarations
class AtomType;
class Isotope;
class Isotopologue;
class Species;
class SpeciesAtom;
class SpeciesBond;
class SpeciesAngle;
class SpeciesTorsion;

// Species Tab
class SpeciesTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	SpeciesTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Species* species);
	~SpeciesTab();
	// Main form declaration
	Ui::SpeciesTab ui;


	/*
	 * Data
	 */
	public:
	// Return tab type
	const char* tabType() const;


	/*
	 * Target
	 */
	private:
	// Species data to display
	Species* species_;


	/*
	 * Update
	 */
	private:
	// SpeciesAtomTable row update function
	void updateAtomTableRow(int row, SpeciesAtom* speciesAtom, bool createItems);
	// IsotopologuesIsotopesTable row update function
	void updateIsotopeTableRow(int row, AtomType* atomType, Isotope* isotope, bool createItems);
	// SpeciesBondTable row update function
	void updateBondTableRow(int row, SpeciesBond* speciesBond, bool createItems);
	// SpeciesAngleTable row update function
	void updateAngleTableRow(int row, SpeciesAngle* speciesAngle, bool createItems);
	// SpeciesTorsionTable row update function
	void updateTorsionTableRow(int row, SpeciesTorsion* speciesTorsion, bool createItems);

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
	// Return currently-selected Isotopologue
	Isotopologue* currentIsotopologue();

	private slots:
	void on_AtomTable_itemChanged(QTableWidgetItem* w);
	void on_IsotopologueList_currentRowChanged(int row);
	void on_IsotopeTable_itemChanged(QTableWidgetItem* w);
	void on_BondTable_itemChanged(QTableWidgetItem* w);
	void on_AngleTable_itemChanged(QTableWidgetItem* w);
	void on_TorsionTable_itemChanged(QTableWidgetItem* w);


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif

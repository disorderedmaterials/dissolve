/*
	*** Species Tab
	*** src/gui/speciestab.h
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

#ifndef DUQ_SPECIESTAB_H
#define DUQ_SPECIESTAB_H

#include "gui/ui_speciestab.h"
#include "gui/maintab.h"

// Forward Declarations
class Species;
class SpeciesAtom;

// Species Tab
class SpeciesTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	SpeciesTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title);
	~SpeciesTab();
	// Main form declaration
	Ui::SpeciesTab ui;


	/*
	 * SubWidget / SubWindow Handling
	 */
	public:
	// Return the tab's SubWindow area, if it has one
	QMdiArea* subWindowArea();
	// Return the tab's SubWidget layout, if it has one
	QLayout* subWidgetLayout();


	/*
	 * Update
	 */
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
	// Return currently-selected Species
	Species* currentSpecies();
	// SpeciesList row update function
	void updateSpeciesListRow(int row, Species* species, bool createItem);
	// SpeciesAtomTable row update function
	void updateSpeciesAtomTableRow(int row, SpeciesAtom* speciesAtom, bool createItem);

	private slots:
	void on_SpeciesList_currentRowChanged(int row);
	void on_SpeciesAtomTable_itemChanged(QTableWidgetItem* w);


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
};

#endif

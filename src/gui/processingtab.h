/*
	*** Processing Tab
	*** src/gui/processingtab.h
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

#ifndef DISSOLVE_PROCESSINGTAB_H
#define DISSOLVE_PROCESSINGTAB_H

#include "gui/ui_processingtab.h"
#include "gui/maintab.h"
#include "gui/widgets/subwidget.h"

// Forward Declarations
class ModuleChart;

// Simulation Tab
class ProcessingTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ProcessingTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title);
	~ProcessingTab();
	// Main form declaration
	Ui::ProcessingTab ui;


	/*
	 * Data
	 */
	public:
	// Return tab type
	const char* tabType() const;


	/*
	 * Widgets
	 */
	private:
	// Chart widget being displayed
	ModuleChart* chartWidget_;


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
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif

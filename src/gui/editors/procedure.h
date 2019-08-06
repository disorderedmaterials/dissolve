/*
	*** Procedure Editor
	*** src/gui/layereditor.h
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

#ifndef DISSOLVE_LAYEREDITOR_H
#define DISSOLVE_LAYEREDITOR_H

#include "gui/ui_layereditor.h"
#include "base/charstring.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Configuration;
class DissolveWindow;
class LineParser;
class MimeTreeWidgetItem;
class ModuleChart;
class ModuleProcedure;
class ModulePalette;

// Procedure Editor
class ProcedureEditor : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ProcedureEditor(QWidget* parent = NULL);
	~ProcedureEditor();
	// Main form declaration
	Ui::ProcedureEditor ui;


	/*
	 * Setup
	 */
	private:
	// Pointer to main window
	DissolveWindow* dissolveWindow_;
	// Target ModuleProcedure
	ModuleProcedure* moduleProcedure_;
	// Parent Configuration (if we are local to one)
	Configuration* localConfiguration_;
	// Module categories and their associated MimeTreeWidgetItems
	RefDataList<MimeTreeWidgetItem,CharString> moduleCategories_;

	public:
	// Set up the ProcedureEditor for the specified ModuleProcedure
	bool setUp(DissolveWindow* dissolveWindow, ModuleProcedure* moduleProcedure, Configuration* localConfiguration = NULL);


	/*
	 * Update
	 */
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;

	public:
	// Update controls
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();


	/*
	 * Widget Functions
	 */
	private:
	// Chart widget being displayed
	ModuleChart* chartWidget_;

	private slots:
	void on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem* item);


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

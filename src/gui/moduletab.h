/*
	*** Module Tab
	*** src/gui/moduletab.h
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

#ifndef DISSOLVE_MODULETAB_H
#define DISSOLVE_MODULETAB_H

#include "gui/ui_moduletab.h"
#include "gui/maintab.h"
#include "gui/widgets/subwidget.h"

// Forward Declarations
class Module;
class ModuleChartModuleBlock;
class QSplitter;

// Module Tab
class ModuleTab : public QWidget, public ListItem<ModuleTab>, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Module* module);
	~ModuleTab();
	// Main form declaration
	Ui::ModuleTab ui;


	/*
	 * Data
	 */
	public:
	// Return tab type
	const char* tabType() const;


	/*
	 * Module Target
	 */
	public:
	// Module displayed in this tab
	Module* module_;
	// Module control widget displayed
	ModuleChartModuleBlock* controlsWidget_;
	// ModuleWidget displayed in this control widget (if any)
	ModuleWidget* moduleWidget_;

	public:
	// Initialise controls for the specified Module
	void initialiseControls(Module* module);
	// Return displayed Module
	const Module* module() const;


	/*
	 * Widgets
	 */
	private:
	// Splitter which contains the controls and module widgets
	QSplitter* splitter_;


	/*
	 * Update
	 */
	public:
	// Update controls in tab
	void updateControls();
	// Disable sensitive controls within tab, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within tab, ready for main code to run
	void enableSensitiveControls();

	public slots:
	// Update controls in module widget only
	void updateModuleWidget();


	/*
	 * State
	 */
	public:
	// Read widget state through specified LineParser
	bool readState(LineParser& parser, const CoreData& coreData);
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
};

#endif

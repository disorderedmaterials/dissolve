/*
	*** Module Widget
	*** src/modules/braggsq/gui/modulewidget.h
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

#ifndef DISSOLVE_BRAGGSQMODULEWIDGET_H
#define DISSOLVE_BRAGGSQMODULEWIDGET_H

#include "modules/braggsq/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class BraggSQModule;
class Configuration;
class Dissolve;
class Module;
class PartialSet;
class DataViewer;

// Module Widget
class BraggSQModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	BraggSQModule* module_;
	// DataViewers contained within this widget
	DataViewer* reflectionsGraph_, *totalsGraph_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	BraggSQModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve);
	~BraggSQModuleWidget();
	// Main form declaration
	Ui::BraggSQModuleWidget ui;
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();


	/*
	 * ModuleWidget Implementations
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widgets / Functions
	 */
	private:
	// Current Configuration whose data is being displayed
	Configuration* currentConfiguration_;

	private:
	// Set data targets in graphs
	void setGraphDataTargets();

	private slots:
	void on_TargetCombo_currentIndexChanged(int index);
};

#endif

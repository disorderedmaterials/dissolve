/*
	*** Module Widget
	*** src/modules/calculate_dangle/gui/modulewidget.h
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

#ifndef DISSOLVE_CALCULATEDANGLEMODULEWIDGET_H
#define DISSOLVE_CALCULATEDANGLEMODULEWIDGET_H

#include "modules/calculate_dangle/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class CalculateDAngleModule;
class DataViewer;

// Module Widget
class CalculateDAngleModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	CalculateDAngleModule* module_;
	// DataViewers contained within this widget
	DataViewer* rdfGraph_, *angleGraph_, *dAngleGraph_;

	public:
	// Constructor
	CalculateDAngleModuleWidget(QWidget* parent, CalculateDAngleModule* module);
	// Main form declaration
	Ui::CalculateDAngleModuleWidget ui;
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget
	void disableSensitiveControls();
	// Enable sensitive controls within widget
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
	// Set data targets in graphs
	void setGraphDataTargets(CalculateDAngleModule* module);

	private slots:
};

#endif

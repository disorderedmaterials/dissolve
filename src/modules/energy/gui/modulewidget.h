/*
	*** Module Widget
	*** src/modules/energy/gui/modulewidget.h
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

#ifndef DISSOLVE_ENERGYMODULEWIDGET_H
#define DISSOLVE_ENERGYMODULEWIDGET_H

#include "modules/energy/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class PartialSet;
class EnergyModule;
class DataViewer;

// Module Widget
class EnergyModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	EnergyModule* module_;
	// DataViewer contained within this widget
	DataViewer* energyGraph_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	EnergyModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve);
	~EnergyModuleWidget();
	// Main form declaration
	Ui::EnergyModuleWidget ui;
	// Update controls within widget
	void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);
	// Disable sensitive controls within widget
	void disableSensitiveControls();
	// Enable sensitive controls within widget
	void enableSensitiveControls();


	/*
	 * ModuleWidget Implementations
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser) const;
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
	void setGraphDataTargets(EnergyModule* module);

	private slots:
	void on_TargetCombo_currentIndexChanged(int index);
};

#endif

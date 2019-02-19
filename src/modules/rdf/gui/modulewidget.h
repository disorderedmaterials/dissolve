/*
	*** Module Widget
	*** src/modules/rdf/gui/modulewidget.h
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

#ifndef DISSOLVE_RDFMODULEWIDGET_H
#define DISSOLVE_RDFMODULEWIDGET_H

#include "modules/rdf/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class RDFModule;
class PartialSet;
class DataViewer;

// Module Widget
class RDFModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	RDFModule* module_;
	// DataViewers contained within this widget
	DataViewer* partialsGraph_, *totalsGraph_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	RDFModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve);
	~RDFModuleWidget();
	// Main form declaration
	Ui::RDFModuleWidget ui;
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
	void setGraphDataTargets(RDFModule* module);

	private slots:
	void on_TargetCombo_currentIndexChanged(int index);
};

#endif

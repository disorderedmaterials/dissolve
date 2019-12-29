/*
	*** Module Widget
	*** src/modules/calculate_rdf/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_CALCULATERDF_H
#define DISSOLVE_MODULEWIDGET_CALCULATERDF_H

#include "modules/calculate_rdf/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class CalculateRDFModule;
class DataViewer;

// Module Widget
class CalculateRDFModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	CalculateRDFModule* module_;
	// DataViewer contained within this widget
	DataViewer* rdfGraph_;

	public:
	// Constructor
	CalculateRDFModuleWidget(QWidget* parent, CalculateRDFModule* module);


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::CalculateRDFModuleWidget ui_;

	public:
	// Update controls within widget
	void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);


	/*
	 * State I/O
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
	// Set data targets in graphs
	void setGraphDataTargets();

	private slots:
};

#endif

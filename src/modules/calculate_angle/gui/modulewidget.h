/*
	*** Module Widget
	*** src/modules/calculate_angle/gui/modulewidget.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_MODULEWIDGET_CALCULATEANGLE_H
#define DISSOLVE_MODULEWIDGET_CALCULATEANGLE_H

#include "gui/modulewidget.h"
#include "modules/calculate_angle/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateAngleModule;
class DataViewer;

// Module Widget
class CalculateAngleModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

      private:
	// Associated Module
	CalculateAngleModule *module_;

      public:
	// Constructor
	CalculateAngleModuleWidget(QWidget *parent, CalculateAngleModule *module);

	/*
	 * UI
	 */
      private:
	// Main form declaration
	Ui::CalculateAngleModuleWidget ui_;
	// DataViewers contained within this widget
	DataViewer *rdfABGraph_, *rdfBCGraph_, *angleGraph_, *dAngleABGraph_, *dAngleBCGraph_;

      public:
	// Update controls within widget
	void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

	/*
	 * State I/O
	 */
      public:
	// Write widget state through specified LineParser
	bool writeState(LineParser &parser) const;
	// Read widget state through specified LineParser
	bool readState(LineParser &parser);

	/*
	 * Widgets / Functions
	 */
      private:
	// Set data targets in graphs
	void setGraphDataTargets(CalculateAngleModule *module);

      private slots:
};

#endif

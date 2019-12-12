/*
	*** Calibration Module Widget
	*** src/modules/calibration/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_CALIBRATION_H
#define DISSOLVE_MODULEWIDGET_CALIBRATION_H

#include "modules/calibration/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class CalibrationModule;
class Dissolve;
class Module;
class PartialSet;
class DataViewer;

// Module Widget
class CalibrationModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	CalibrationModule* module_;
	// DataViewer contained within this widget
	DataViewer* dataView_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	CalibrationModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve);
	~CalibrationModuleWidget();


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::CalibrationModuleWidget ui_;

	public:
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
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widgets / Functions
	 */
	private:
};

#endif

/*
	*** Calibration Module Widget - Functions
	*** src/modules/calibrate/gui/modulewidget_funcs.cpp
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

#include "modules/calibrate/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/calibrate/calibrate.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
CalibrationModuleWidget::CalibrationModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<CalibrationModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our DataViewer widget
	dataView_ = ui.PlotWidget->dataViewer();

	// Set up the view
	View& view = dataView_->view();
	view.setViewType(View::FlatXYView);
	view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	view.axes().setMax(0, 10.0);
	view.axes().setTitle(1, "g(r) / S(Q)");
	view.axes().setMin(1, -1.0);
	view.axes().setMax(1, 1.0);

	refreshing_ = false;
}

CalibrationModuleWidget::~CalibrationModuleWidget()
{
}

// Update controls within widget
void CalibrationModuleWidget::updateControls()
{
	dataView_->postRedisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void CalibrationModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void CalibrationModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool CalibrationModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer session
	if (!dataView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool CalibrationModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer session
	if (!dataView_->readSession(parser)) return false;
	
	return true;
}

/*
 * Widgets / Functions
 */

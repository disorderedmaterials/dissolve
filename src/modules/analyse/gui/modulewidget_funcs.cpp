/*
	*** Analyse Module Widget - Functions
	*** src/modules/analyse/gui/modulewidget_funcs.cpp
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

#include "modules/analyse/gui/modulewidget.h"
#include "modules/analyse/analyse.h"

// Constructor
AnalyseModuleWidget::AnalyseModuleWidget(QWidget* parent, Module* module, const CoreData& coreData) : ModuleWidget(parent), module_(dynamic_cast<AnalyseModule*>(module))
{
	// Set up user interface
	ui.setupUi(this);

	// Set Procedure target
	ui.EditorWidget->setUp(&module_->analyser(), coreData);

	refreshing_ = false;
}

// Update controls within widget
void AnalyseModuleWidget::updateControls()
{
}

// Disable sensitive controls within widget
void AnalyseModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void AnalyseModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool AnalyseModuleWidget::writeState(LineParser& parser)
{
	return false;
}

// Read widget state through specified LineParser
bool AnalyseModuleWidget::readState(LineParser& parser)
{
	return false;
}

/*
 * Widgets / Functions
 */


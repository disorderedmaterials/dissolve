/*
	*** SanityCheck Module Widget - Functions
	*** src/modules/sanitycheck/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/sanitycheck/gui/modulewidget.h"

// Constructor
SanityCheckModuleWidget::SanityCheckModuleWidget(QWidget* parent, Module* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = false;
}

// Update controls within widget
void SanityCheckModuleWidget::updateControls()
{
}

// Disable sensitive controls within widget, ready for main code to run
void SanityCheckModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void SanityCheckModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool SanityCheckModuleWidget::writeState(LineParser& parser)
{
	return false;
}

// Read widget state through specified LineParser
bool SanityCheckModuleWidget::readState(LineParser& parser)
{
	return false;
}

/*
 * Widgets / Functions
 */


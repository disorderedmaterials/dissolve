/*
	*** AtomShake Module Widget - Functions
	*** src/modules/atomshake/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/atomshake/gui/modulewidget.h"

// Constructor
AtomShakeModuleWidget::AtomShakeModuleWidget(QWidget* parent, Module* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = false;
}

// Destructor
AtomShakeModuleWidget::~AtomShakeModuleWidget()
{
}

// Update controls within widget
void AtomShakeModuleWidget::updateControls()
{
}

// Disable sensitive controls within widget, ready for main code to run
void AtomShakeModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void AtomShakeModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool AtomShakeModuleWidget::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool AtomShakeModuleWidget::readState(LineParser& parser)
{
	return true;
}

/*
 * Widgets / Functions
 */


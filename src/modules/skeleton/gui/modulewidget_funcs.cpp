/*
	*** Skeleton Module Widget - Functions
	*** src/modules/skeleton/gui/modulewidget_funcs.cpp
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

#include "modules/skeleton/gui/modulewidget.h"

// Constructor
SkeletonModuleWidget::SkeletonModuleWidget(QWidget* parent, Module* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void SkeletonModuleWidget::updateControls(int flags)
{
}

// Disable sensitive controls within widget
void SkeletonModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void SkeletonModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool SkeletonModuleWidget::writeState(LineParser& parser) const
{
	return false;
}

// Read widget state through specified LineParser
bool SkeletonModuleWidget::readState(LineParser& parser)
{
	return false;
}

/*
 * Widgets / Functions
 */


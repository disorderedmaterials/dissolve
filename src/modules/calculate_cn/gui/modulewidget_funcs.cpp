/*
	*** CalculateCoordinationNumber Module Widget - Functions
	*** src/modules/calculate_cn/gui/modulewidget_funcs.cpp
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

#include "modules/calculate_cn/gui/modulewidget.h"
#include "modules/calculate_cn/cn.h"

// Constructor
CalculateCoordinationNumberModuleWidget::CalculateCoordinationNumberModuleWidget(QWidget* parent, CalculateCoordinationNumberModule* cnModule) : ModuleWidget(parent), module_(cnModule)
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = false;
}

// Update controls within widget
void CalculateCoordinationNumberModuleWidget::updateControls(int flags)
{
	// Update CN labels
	if (module_)
	{
		ui_.RegionAResultFrame->setText(module_->coordinationNumber(0));
		ui_.RegionBResultFrame->setText(module_->coordinationNumber(1));
		ui_.RegionCResultFrame->setText(module_->coordinationNumber(2));
	}
	else
	{
		ui_.RegionAResultFrame->setText(SampledDouble());
		ui_.RegionBResultFrame->setText(SampledDouble());
		ui_.RegionCResultFrame->setText(SampledDouble());
	}
}

// Disable sensitive controls within widget
void CalculateCoordinationNumberModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void CalculateCoordinationNumberModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool CalculateCoordinationNumberModuleWidget::writeState(LineParser& parser)
{
	return false;
}

// Read widget state through specified LineParser
bool CalculateCoordinationNumberModuleWidget::readState(LineParser& parser)
{
	return false;
}

/*
 * Widgets / Functions
 */


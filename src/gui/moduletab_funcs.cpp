/*
	*** ModuleTab Functions
	*** src/gui/moduletab_funcs.cpp
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

#include "gui/moduletab.h"
#include "main/duq.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleTab::ModuleTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, Module* module) : MainTab(duqWindow, duq, parent, module->uniqueName(), this), module_(module)
{
	ui.setupUi(this);

	refreshing_ = false;

	// Set up our keywords widget
	ui.KeywordsFrame->setUp(duqWindow_, module_);

	updateControls();
}

ModuleTab::~ModuleTab()
{
}

// Return the tab's SubWindow area, if it has one
QMdiArea* ModuleTab::subWindowArea()
{
	return NULL;
}

// Return the tab's SubWidget layout, if it has one
QLayout* ModuleTab::subWidgetLayout()
{
	return NULL;
}

/*
 * Update
 */

// Update controls in tab
void ModuleTab::updateControls()
{
	if (!module_) return;

	// Update header labels
	CharString topText("%s (%s)", module_->name(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	CharString bottomText("Runs @ %s", module_->frequencyDetails(duq_.iteration()));
	ui.BottomLabel->setText(bottomText.get());

	// Make sure tooltip on HeaderFrame is up-to-date
	CharString toolTip("Targets: ");
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		if (configIterator.first()) toolTip.strcatf("%s", cfg->name());
		else toolTip.strcatf(", %s", cfg->name());
	}
	ui.HeaderFrame->setToolTip(toolTip.get());

	// Set button status
	ui.EnabledButton->setChecked(module_->enabled());
}

// Disable sensitive controls within tab, ready for main code to run
void ModuleTab::disableSensitiveControls()
{
	// Disable control buttons
	ui.RunButton->setEnabled(false);

	// Disable keywords widget
	ui.KeywordsFrame->setEnabled(false);
}

// Enable sensitive controls within tab, ready for main code to run
void ModuleTab::enableSensitiveControls()
{
	// Enable control buttons
	ui.RunButton->setEnabled(true);

	// Enable keywords widget
	ui.KeywordsFrame->setEnabled(true);
}

/*
 * State
 */

// Write widget state through specified LineParser
bool ModuleTab::writeState(LineParser& parser)
{
	// Loop over our SubWidgets
	ListIterator<SubWidget> subWidgetIterator(subWidgets_);
	while (SubWidget* subWidget = subWidgetIterator.iterate())
	{
		// Write window state
		if (!parser.writeLineF("'%s'  %s  '%s'\n", title_.get(), subWidget->widgetType(), subWidget->title())) return false;
		if (!parser.writeLineF("0\n")) return false;
		if (!subWidget->writeState(parser)) return false;
	}

	return true;
}

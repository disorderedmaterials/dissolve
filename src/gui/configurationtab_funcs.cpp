/*
	*** ConfigurationTab Functions
	*** src/gui/configurationtab_funcs.cpp
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

#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "classes/configuration.h"
#include "module/module.h"

// Constructor / Destructor
ConfigurationTab::ConfigurationTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, Configuration* cfg) : MainTab(duq, parent, CharString("Configuration: %s", title), this)
{
	ui.setupUi(this);

	configuration_ = cfg;

	// Add module widgets to scroll area
	RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
	while (Module* module = moduleIterator.iterate())
	{
		ModuleControlWidget* moduleWidget = new ModuleControlWidget(NULL, module, duq_, CharString("%s (%s)", module->name(), module->uniqueName()));
		connect(moduleWidget, SIGNAL(moduleRun()), duqWindow, SLOT(updateControls()));
		ui.ModuleWidgetLayout->addWidget(moduleWidget);
	}
}

ConfigurationTab::~ConfigurationTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return whether the tab has a SubWindow area
bool ConfigurationTab::hasSubWindowArea()
{
	return false;
}

// Add SubWindow for widget containing specified data (as pointer)
QMdiSubWindow* ConfigurationTab::addSubWindow(SubWidget* widget, void* windowContents)
{
	Messenger::error("ConfigurationTab doesn't have an MDI area, so don't try to add a SubWindow!\n");
	return NULL;
}

// Find and return named SubWidget
SubWidget* ConfigurationTab::findSubWidget(const char* widgetTitle)
{
	ListIterator<SubWidget> widgetIterator(subWidgets_);
	while (SubWidget* subWidget = widgetIterator.iterate()) if (DUQSys::sameString(widgetTitle, subWidget->title())) return subWidget;
	return NULL;
}

/*
 * Update
 */

// Update controls in tab
void ConfigurationTab::updateControls()
{
}

// Disable sensitive controls within tab, ready for main code to run
void ConfigurationTab::disableSensitiveControls()
{
}

// Enable sensitive controls within tab, ready for main code to run
void ConfigurationTab::enableSensitiveControls()
{
}

/*
 * State
 */

// Write widget state through specified LineParser
bool ConfigurationTab::writeState(LineParser& parser)
{
	return true;
}

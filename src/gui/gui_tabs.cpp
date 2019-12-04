/*
	*** Dissolve GUI - Tabs
	*** src/gui/gui_tabs.cpp
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

#include "gui/gui.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/layertab.h"
#include "gui/moduletab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include <QInputDialog>

/*
 * Private Slots
 */

void DissolveWindow::on_MainTabs_currentChanged(int index)
{
	if (refreshing_) return;

	// Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets are derived
	MainTab* currentTab = dynamic_cast<MainTab*>(ui_.MainTabs->widget(index));
	if (!currentTab)
	{
		Messenger::print("Can't cast this tab widget (index %i) into a MainTab.\n", index);
		return;
	}

	// Update contents
	currentTab->updateControls();

	// Update menu items
	updateMenus();
}

/*
 * Public Slots
 */

// Add or go to Module tab for the Module with the unique name provided
void DissolveWindow::showModuleTab(const QString& uniqueName)
{
	// Find the Module
	Module* module = dissolve_.findModuleInstance(qPrintable(uniqueName));
	if (!module) return;

	ui_.MainTabs->addModuleTab(this, module);
}

// Remove the Module tab (if it exists) for the Module with the unique name provided
void DissolveWindow::removeModuleTab(const QString& uniqueName)
{
	// Find the Module
	Module* module = dissolve_.findModuleInstance(qPrintable(uniqueName));
	if (!module) return;

	ui_.MainTabs->removeModuleTab(module);
}

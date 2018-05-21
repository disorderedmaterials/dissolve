/*
	*** dUQ Main Window - Tab Management
	*** src/gui/gui_tabs.cpp
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

#include "gui/gui.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/processingtab.h"
#include "gui/simulationtab.h"
#include "gui/speciestab.h"
#include "gui/systemtab.h"
#include "gui/workspacetab.h"
#include "classes/configuration.h"
#include <QInputDialog>

void DUQWindow::on_MainTabs_currentChanged(int index)
{
	if (refreshing_) return;

	// Current tab index changed
	if (index >= tabs_.nItems())
	{
		Messenger::error("Something has gone wrong - tab index changed to %i, but there are only %i in the list.\n", index, tabs_.nItems());
		return;
	}

	MainTab* currentTab = tabs_[index];

	// Set controls in Workspace menu
	// -- Workspace name
	ui.WorkspaceCurrentAction->setText(index == -1 ? "<No Current Workspace>" : tabs_[index]->title());
	// -- Disable controls if the current tab has no valid SubWindow (MDI) area
	ui.WorkspaceAddWidgetAction->setEnabled(currentTab->subWindowArea());

	// Update contents
	currentTab->updateControls();
}

void DUQWindow::mainTabsDoubleClicked(int index)
{
	if (index == -1) return;

	// We can only rename workspace-type tabs
	MainTab* tab = tabs_[index];
	if (!tab) return;

	// Rename the current workspace
	bool ok;
	QString text = QInputDialog::getText(this, "Rename Workspace", "Enter the new name of the workspace", QLineEdit::Normal, tab->title(), &ok);
	if (!ok || text.isEmpty()) return;

	// Ensure that the name provided is unique
	CharString newName = qPrintable(text);
	int count = 0;
	while (findTab(newName)) newName.sprintf("%s%02i", qPrintable(text), ++count);

	tab->setTitle(newName);
}

// Clear all tabs, except the "Setup" tab
void DUQWindow::clearAllTabs()
{
	// Delete all our referenced tabs - removal of the tab and widget will be handled by the destructor
	tabs_.clear();
}

// Add core tabs
void DUQWindow::addCoreTabs()
{
	MainTab* forcefieldTab = new ForcefieldTab(this, duq_, ui.MainTabs, "Forcefield");
	tabs_.own(forcefieldTab);

	MainTab* speciesTab = new SpeciesTab(this, duq_, ui.MainTabs, "Species");
	tabs_.own(speciesTab);

	MainTab* systemTab = new SystemTab(this, duq_, ui.MainTabs, "System");
	tabs_.own(systemTab);

	MainTab* simulationTab = new SimulationTab(this, duq_, ui.MainTabs, "Simulation");
	tabs_.own(simulationTab);
}

// Add tab for specified Configuration target
void DUQWindow::addConfigurationTab(Configuration* cfg)
{
	MainTab* tab = new ConfigurationTab(this, duq_, ui.MainTabs, cfg->name(), cfg);
	tabs_.own(tab);
}

// Add processing workspace
void DUQWindow::addProcessingTab()
{
	MainTab* tab = new ProcessingTab(this, duq_, ui.MainTabs, "Processing");
	tabs_.own(tab);
}

// Add on an empty workspace tab
MainTab* DUQWindow::addWorkspaceTab(const char* title)
{
	// Check that a tab with this title doesn't already exist
	MainTab* tab = findTab(title);
	if (!tab)
	{
		tab = new WorkspaceTab(this, duq_, ui.MainTabs, title);
		tabs_.own(tab);
	}
	else Messenger::printVerbose("Tab '%s' already exists, so returning that instead...\n", title);

	return tab;
}

// Find tab with title specified
MainTab* DUQWindow::findTab(const char* title)
{
	for (MainTab* tab = tabs_.first() ; tab != NULL; tab = tab->next) if (DUQSys::sameString(title, tab->title())) return tab;

	return NULL;
}

// Return current tab
MainTab* DUQWindow::currentTab()
{
	if (ui.MainTabs->currentIndex() == -1) return NULL;

	return tabs_[ui.MainTabs->currentIndex()];
}

// Make specified tab the current one
void DUQWindow::setCurrentTab(MainTab* tab)
{
	ui.MainTabs->setCurrentIndex(tabs_.indexOf(tab));
}

// Make specified tab the current one (by index)
void DUQWindow::setCurrentTab(int tabIndex)
{
	ui.MainTabs->setCurrentIndex(tabIndex);
}

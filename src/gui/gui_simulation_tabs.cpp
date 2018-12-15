/*
	*** Dissolve GUI - Simulation Page - Tabs
	*** src/gui/gui_simulation_tabs.cpp
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

#include "gui/gui.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/moduletab.h"
#include "gui/processingtab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include <QInputDialog>

void DissolveWindow::on_MainTabs_currentChanged(int index)
{
	if (refreshing_) return;

	// Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets are derived
	MainTab* currentTab = dynamic_cast<MainTab*>(ui.MainTabs->widget(index));
	if (!currentTab)
	{
		Messenger::print("Can't cast this tab widget (index %i) into a MainTab.\n", index);
		return;
	}

	// Update contents
	currentTab->updateControls();
}

void DissolveWindow::mainTabsDoubleClicked(int index)
{
	if (index == -1) return;

// 	// We can only rename workspace-type tabs
// 	MainTab* tab = tabs_[index];
// 	if (!tab) return;
// 	if (!tab->canChangeTitle()) return;
// 
// 	// Rename the current workspace
// 	bool ok;
// 	QString text = QInputDialog::getText(this, "Rename Workspace", "Enter the new name of the workspace", QLineEdit::Normal, tab->title(), &ok);
// 	if (!ok || text.isEmpty()) return;
// 
// 	// Ensure that the name provided is unique
// 	CharString newName = qPrintable(text);
// 	int count = 0;
// 	while (findTab(newName)) newName.sprintf("%s%02i", qPrintable(text), ++count);
// 
// 	tab->setTitle(newName);
}

// Remove tabs related to the current data
void DissolveWindow::clearTabs()
{
	// Removal of the tab and widget will be handled by the destructors
	speciesTabs_.clear();
	configurationTabs_.clear();
	moduleTabs_.clear();
}

// Reconcile tabs, making them consistent with the current data
void DissolveWindow::reconcileTabs()
{
	// Species - Global tab indices run from 1 (first tab after ForcefieldTab) to 1+nSpecies
	ListIterator<Species> speciesIterator(dissolve_.species());
	int currentTabIndex = 0;
	while (Species* sp = speciesIterator.iterate())
	{
		// Loop over existing tabs
		while (currentTabIndex < speciesTabs_.nItems())
		{
			// If the existing tab is displaying the current Species already, then we can move on. Otherwise, delete it
			if (speciesTabs_[currentTabIndex]->species() == sp) break;
			else speciesTabs_.remove(currentTabIndex);
		}

		// If the current tab index is (now) out of range, add a new one
		if (currentTabIndex == speciesTabs_.nItems())
		{
			SpeciesTab* newTab = new SpeciesTab(this, dissolve_, ui.MainTabs, sp->name(), sp);
			speciesTabs_.own(newTab);
			ui.MainTabs->insertTab(1 + currentTabIndex, newTab, sp->name());
			ui.MainTabs->setTabTextColour(newTab->page(), QColor(0, 81, 0));
			ui.MainTabs->setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_species.svg"));
		}

		++currentTabIndex;
	}

	// Configurations - Global tab indices run from 1+nSpecies (first tab after last Species) to 1+nSpecies+nConfigurations
	ListIterator<Configuration> configurationIterator(dissolve_.configurations());
	currentTabIndex = 0;
	while (Configuration* cfg = configurationIterator.iterate())
	{
		// Loop over existing tabs
		while (currentTabIndex < configurationTabs_.nItems())
		{
			// If the existing tab is displaying the current Configuration already, then we can move on. Otherwise, delete it
			if (configurationTabs_[currentTabIndex]->configuration() == cfg) break;
			else configurationTabs_.remove(currentTabIndex);
		}

		// If the current tab index is (now) out of range, add a new one
		if (currentTabIndex == configurationTabs_.nItems())
		{
			ConfigurationTab* newTab = new ConfigurationTab(this, dissolve_, ui.MainTabs, cfg->name(), cfg);
			configurationTabs_.own(newTab);
			ui.MainTabs->insertTab(1 + dissolve_.nSpecies() + currentTabIndex, newTab, cfg->name());
			ui.MainTabs->setTabTextColour(newTab->page(), QColor(0, 81, 0));
			ui.MainTabs->setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_configuration.svg"));
		}

		++currentTabIndex;
	}
}

// Add core (permanent) tabs
void DissolveWindow::addCoreTabs()
{
	// Forcefield
	forcefieldTab_ = new ForcefieldTab(this, dissolve_, ui.MainTabs, "Forcefield");
	ui.MainTabs->addTab(forcefieldTab_->page(), "Forcefield");
	ui.MainTabs->setTabTextColour(forcefieldTab_->page(), QColor(189, 68, 0));
	ui.MainTabs->setTabIcon(forcefieldTab_->page(), QIcon(":/tabs/icons/tabs_ff.svg"));
	
	// Main Processing
	mainProcessingTab_ = new ProcessingTab(this, dissolve_, ui.MainTabs, "Main Processing");
	ui.MainTabs->addTab(mainProcessingTab_->page(), "Main Processing");
	ui.MainTabs->setTabTextColour(mainProcessingTab_->page(), QColor(11, 36, 118));
	ui.MainTabs->setTabIcon(mainProcessingTab_->page(), QIcon(":/tabs/icons/tabs_flow.svg"));
}

// Add on an empty workspace tab
MainTab* DissolveWindow::addWorkspaceTab(const char* title)
{
	// Check that a tab with this title doesn't already exist
	MainTab* tab = findTab(title);
	if (!tab)
	{
		WorkspaceTab* newWorkspace = new WorkspaceTab(this, dissolve_, ui.MainTabs, title);
		workspaceTabs_.own(newWorkspace);
		return newWorkspace;
	}
	else Messenger::printVerbose("Tab '%s' already exists, so returning that instead...\n", title);

	return tab;
}

// Find tab with title specified
MainTab* DissolveWindow::findTab(const char* title)
{
	if (DissolveSys::sameString(title, "Forcefield")) return forcefieldTab_;

	for (SpeciesTab* tab = speciesTabs_.first(); tab != NULL; tab = tab->next) if (DissolveSys::sameString(title, tab->title())) return tab;
	for (ConfigurationTab* tab = configurationTabs_.first(); tab != NULL; tab = tab->next) if (DissolveSys::sameString(title, tab->title())) return tab;
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next) if (DissolveSys::sameString(title, tab->title())) return tab;
	for (WorkspaceTab* tab = workspaceTabs_.first(); tab != NULL; tab = tab->next) if (DissolveSys::sameString(title, tab->title())) return tab;

	return NULL;
}

// Find tab with specified page widget
MainTab* DissolveWindow::findTab(QWidget* page)
{
	if (forcefieldTab_ && (forcefieldTab_->page() == page)) return forcefieldTab_;

	MainTab* result = speciesTab(page);
	if (!result) result = configurationTab(page);
	if (!result) result = moduleTab(page);
	if (!result) result = workspaceTab(page);

	return result;
}

// Find SpeciesTab containing specified page widget
SpeciesTab* DissolveWindow::speciesTab(QWidget* page)
{
	for (SpeciesTab* tab = speciesTabs_.first(); tab != NULL; tab = tab->next) if (tab->page() == page) return tab;

	return NULL;
}

// Find ConfigurationTab containing specified page widget
ConfigurationTab* DissolveWindow::configurationTab(QWidget* page)
{
	for (ConfigurationTab* tab = configurationTabs_.first(); tab != NULL; tab = tab->next) if (tab->page() == page) return tab;

	return NULL;
}

// Find ModuleTab containing specified page widget
ModuleTab* DissolveWindow::moduleTab(QWidget* page)
{
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next) if (tab->page() == page) return tab;

	return NULL;
}

// Find ModuleTab containing specified Module
ModuleTab* DissolveWindow::moduleTab(Module* module)
{
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next) if (tab->module() == module) return tab;

	return NULL;
}

// Find WorkspaceTab containing specified page widget
WorkspaceTab* DissolveWindow::workspaceTab(QWidget* page)
{
	for (WorkspaceTab* tab = workspaceTabs_.first(); tab != NULL; tab = tab->next) if (tab->page() == page) return tab;

	return NULL;
}

// Return current tab
MainTab* DissolveWindow::currentTab()
{
	if (ui.MainTabs->currentWidget() == NULL) return NULL;

	// Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets are derived
	MainTab* currentTab = dynamic_cast<MainTab*>(ui.MainTabs->currentWidget());
	if (!currentTab)
	{
		Messenger::print("Can't cast current tab (index %i) into a MainTab.\n", ui.MainTabs->currentIndex());
		return NULL;
	}

	return currentTab;
}

// Make specified tab the current one
void DissolveWindow::setCurrentTab(MainTab* tab)
{
	ui.MainTabs->setCurrentWidget(tab->page());
}

// Make specified tab the current one (by index)
void DissolveWindow::setCurrentTab(int tabIndex)
{
	ui.MainTabs->setCurrentIndex(tabIndex);
}

// Return reference list of all current tabs
RefList<MainTab,bool> DissolveWindow::allTabs() const
{
	RefList<MainTab,bool> tabs;

	tabs.add(forcefieldTab_);
	tabs.add(mainProcessingTab_);
	for (SpeciesTab* tab = speciesTabs_.first(); tab != NULL; tab = tab->next) tabs.add(tab);
	for (ConfigurationTab* tab = configurationTabs_.first(); tab != NULL; tab = tab->next) tabs.add(tab);
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next) tabs.add(tab);
	for (WorkspaceTab* tab = workspaceTabs_.first(); tab != NULL; tab = tab->next) tabs.add(tab);

	return tabs;
}

// Create / go to Module tab for specified Module, provided it has a Module control widget
MainTab* DissolveWindow::addModuleTab(Module* module)
{
	// Does a tab for this Module already exist
	ModuleTab* tab = moduleTab(module);
	if (!tab)
	{
		// Need to create a new ModuleTab
		tab = new ModuleTab(this, dissolve_, ui.MainTabs, module->uniqueName(), module);
		moduleTabs_.own(tab);
		ui.MainTabs->addTab(tab->page(), module->uniqueName());

		// Add a close button
		ui.MainTabs->addTabCloseButton(tab->page());
	}

	setCurrentTab(tab);

	return tab;
}

// Remove tab containing the specified page widget
void DissolveWindow::removeTab(QWidget* page)
{
	// Delete the tab from the tabwidget first - find its index (based on the page widget pointer) and remove that
	int indexToRemove = ui.MainTabs->indexOf(page);
	if (indexToRemove == -1) printf("Couldn't remove tab since its page widget (%p) could not be found.\n", page);
	else ui.MainTabs->removeTab(indexToRemove);

	// Now delete the tab from its list - this will delete the actual page widget
	if (speciesTab(page)) speciesTabs_.remove(speciesTab(page));
	else if (configurationTab(page)) configurationTabs_.remove(configurationTab(page));
	else if (moduleTab(page)) moduleTabs_.remove(moduleTab(page));
	else if (workspaceTab(page)) workspaceTabs_.remove(workspaceTab(page));
	
	printf("Couldn't remove tab %p as it could not be found in any list.\n", page);
}

/*
	*** Main Tabs Widget
	*** src/gui/maintabswidget_funcs.cpp
	Copyright T. Youngs 2013-2020

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
#include "gui/maintabswidget.hui"
#include "gui/maintabsbar.hui"
#include "gui/maintab.h"
#include "gui/charts/moduleblock.h"
#include "main/dissolve.h"
#include "base/messenger.h"
#include <QToolButton>

// Constructor
MainTabsWidget::MainTabsWidget(QWidget* parent) : QTabWidget(parent)
{
	// Create our own tab bar
	mainTabsBar_ = new MainTabsBar(this);
	setTabBar(mainTabsBar_);
	connect(mainTabsBar_, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(tabBarDoubleClicked(int)));

	// Always show scroll buttons when there are many tabs
	setUsesScrollButtons(true);
	mainTabsBar_->setUsesScrollButtons(true);
}

MainTabsWidget::~MainTabsWidget()
{
	// Need to clear tabs in the right order, as data in one can depend on another
	workspaceTabs_.clear();
	moduleTabs_.clear();
	processingLayerTabs_.clear();
	configurationTabs_.clear();
	speciesTabs_.clear();
	allTabs_.clear();
}

/*
 * Tab Data
 */

// Return reference list of all current tabs
RefList<const MainTab> MainTabsWidget::allTabs() const
{
	RefList<const MainTab> constTabs;
	for (MainTab* tab : allTabs_) constTabs.append(tab);

	return constTabs;
}

// Return currently-selected Species (if a SpeciesTab is the current one)
Species* MainTabsWidget::currentSpecies() const
{
	// Get the currently-selected tab, and make sure it's a SpeciesTab
	MainTab* tab = currentTab();
	if (tab->type() != MainTab::SpeciesTabType) return NULL;

	SpeciesTab* speciesTab = dynamic_cast<SpeciesTab*>(tab);
	return (speciesTab ? speciesTab->species() : NULL);
}

// Return currently-selected Configuration (if a ConfigurationTab is the current one)
Configuration* MainTabsWidget::currentConfiguration() const
{
	// Get the currently-selected tab, and make sure it's a SpeciesTab
	MainTab* tab = currentTab();
	if (tab->type() != MainTab::ConfigurationTabType) return NULL;

	ConfigurationTab* configurationTab = dynamic_cast<ConfigurationTab*>(tab);
	return (configurationTab ? configurationTab->configuration() : NULL);
}

// Return currently-selected ModuleLayer (if a LayerTab is the current one)
ModuleLayer* MainTabsWidget::currentLayer() const
{
	// Get the currently-selected tab, and make sure it's a SpeciesTab
	MainTab* tab = currentTab();
	if (tab->type() != MainTab::LayerTabType) return NULL;

	LayerTab* layerTab = dynamic_cast<LayerTab*>(tab);
	return (layerTab ? layerTab->moduleLayer() : NULL);
}

// Find SpeciesTab containing specified page widget
SpeciesTab* MainTabsWidget::speciesTab(QWidget* page)
{
	for (SpeciesTab* tab = speciesTabs_.first(); tab != NULL; tab = tab->next()) if (tab->page() == page) return tab;

	return NULL;
}

// Find ConfigurationTab containing specified page widget
ConfigurationTab* MainTabsWidget::configurationTab(QWidget* page)
{
	for (ConfigurationTab* tab = configurationTabs_.first(); tab != NULL; tab = tab->next()) if (tab->page() == page) return tab;

	return NULL;
}

// Find LayerTab containing specified page widget
LayerTab* MainTabsWidget::processingLayerTab(QWidget* page)
{
	for (LayerTab* tab = processingLayerTabs_.first(); tab != NULL; tab = tab->next()) if (tab->page() == page) return tab;

	return NULL;
}

// Find ModuleTab containing specified page widget
ModuleTab* MainTabsWidget::moduleTab(QWidget* page)
{
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next()) if (tab->page() == page) return tab;

	return NULL;
}

// Find ModuleTab containing specified Module
ModuleTab* MainTabsWidget::moduleTab(Module* module)
{
	for (ModuleTab* tab = moduleTabs_.first(); tab != NULL; tab = tab->next()) if (tab->module() == module) return tab;

	return NULL;
}

// Find WorkspaceTab containing specified page widget
WorkspaceTab* MainTabsWidget::workspaceTab(QWidget* page)
{
	for (WorkspaceTab* tab = workspaceTabs_.first(); tab != NULL; tab = tab->next()) if (tab->page() == page) return tab;

	return NULL;
}

// Find tab with title specified
MainTab* MainTabsWidget::findTab(const char* title)
{
	for (MainTab* tab : allTabs_) if (DissolveSys::sameString(tab->title(), title)) return tab;

	return NULL;
}

// Find tab with specified page widget
MainTab* MainTabsWidget::findTab(QWidget* page)
{
	for (MainTab* tab : allTabs_) if (tab->page() == page) return tab;

	return NULL;
}

// Generate unique tab name with base name provided
const char* MainTabsWidget::uniqueTabName(const char* base)
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findTab(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

/*
 * Tab Management
 */

// Add core tabs
void MainTabsWidget::addCoreTabs(DissolveWindow* dissolveWindow)
{
	// Forcefield
	forcefieldTab_ = new ForcefieldTab(dissolveWindow, dissolveWindow->dissolve(), this, "Forcefield");
	addTab(forcefieldTab_->page(), "Forcefield");
	setTabIcon(forcefieldTab_->page(), QIcon(":/tabs/icons/tabs_ff.svg"));

	allTabs_.append(forcefieldTab_);
}

// Remove tabs related to the current data
void MainTabsWidget::clearTabs()
{
	// Empty our list of close button references
	closeButtons_.clear();

	// Removal of the tab and widget will be handled by the class destructors
	workspaceTabs_.clear();
	moduleTabs_.clear();
	processingLayerTabs_.clear();
	configurationTabs_.clear();
	speciesTabs_.clear();

	// Only remaining tab is the ForcefieldTab...
	allTabs_.clear();
	allTabs_.append(forcefieldTab_);
}

// Reconcile tabs, making them consistent with the provided data
void MainTabsWidget::reconcileTabs(DissolveWindow* dissolveWindow)
{
	Dissolve& dissolve = dissolveWindow->dissolve();

	// Species - Global tab indices run from 1 (first tab after ForcefieldTab) to 1+nSpecies
	ListIterator<Species> speciesIterator(dissolve.species());
	int currentTabIndex = 0;
	int baseIndex = 1;
	while (Species* sp = speciesIterator.iterate())
	{
		// Loop over existing tabs
		while (currentTabIndex < speciesTabs_.nItems())
		{
			// If the existing tab is displaying the current Species already, then we can move on. Otherwise, delete it
			if (speciesTabs_[currentTabIndex]->species() == sp) break;
			else
			{
				allTabs_.remove(speciesTabs_[currentTabIndex]);
				speciesTabs_.remove(currentTabIndex);
			}
		}

		// If the current tab index is (now) out of range, add a new one
		if (currentTabIndex == speciesTabs_.nItems())
		{
			SpeciesTab* newTab = new SpeciesTab(dissolveWindow, dissolve, this, sp->name(), sp);
			speciesTabs_.own(newTab);
			allTabs_.append(newTab);
			insertTab(baseIndex + currentTabIndex, newTab, sp->name());
			addTabCloseButton(newTab->page());
			setTabTextColour(newTab->page(), QColor(0, 81, 0));
			setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_species.svg"));
		}

		++currentTabIndex;
	}
	baseIndex += dissolve.nSpecies();

	// Configurations - Global tab indices run from 1+nSpecies (first tab after last Species) to 1+nSpecies+nConfigurations
	ListIterator<Configuration> configurationIterator(dissolve.configurations());
	currentTabIndex = 0;
	while (Configuration* cfg = configurationIterator.iterate())
	{
		// Loop over existing tabs
		while (currentTabIndex < configurationTabs_.nItems())
		{
			// If the existing tab is displaying the current Configuration already, then we can move on. Otherwise, delete it
			if (configurationTabs_[currentTabIndex]->configuration() == cfg) break;
			else
			{
				allTabs_.remove(configurationTabs_[currentTabIndex]);
				configurationTabs_.remove(currentTabIndex);
			}
		}

		// If the current tab index is (now) out of range, add a new one
		if (currentTabIndex == configurationTabs_.nItems())
		{
			ConfigurationTab* newTab = new ConfigurationTab(dissolveWindow, dissolve, this, cfg->name(), cfg);
			configurationTabs_.own(newTab);
			allTabs_.append(newTab);
			insertTab(baseIndex + currentTabIndex, newTab, cfg->name());
			addTabCloseButton(newTab->page());
			setTabTextColour(newTab->page(), QColor(0, 81, 0));
			setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_configuration.svg"));
		}

		++currentTabIndex;
	}
	baseIndex += dissolve.nConfigurations();

	// Processing Layers - Global tab indices run from 1+nSpecies+nConfigurations (first tab after last Configuration) to 1+nSpecies+nConfigurations+nProcessingLayers
	ListIterator<ModuleLayer> processingLayerIterator(dissolve.processingLayers());
	currentTabIndex = 0;
	while (ModuleLayer* layer = processingLayerIterator.iterate())
	{
		// Loop over existing tabs
		while (currentTabIndex < processingLayerTabs_.nItems())
		{
			// If the existing tab is displaying the current ModuleLayer already, then we can move on. Otherwise, delete it
			if (processingLayerTabs_[currentTabIndex]->moduleLayer() == layer) break;
			else
			{
				allTabs_.remove(processingLayerTabs_[currentTabIndex]);
				processingLayerTabs_.remove(currentTabIndex);
			}
		}

		// If the current tab index is (now) out of range, add a new one
		if (currentTabIndex == processingLayerTabs_.nItems())
		{
			LayerTab* newTab = new LayerTab(dissolveWindow, dissolve, this, layer->name(), layer);
			processingLayerTabs_.own(newTab);
			allTabs_.append(newTab);
			insertTab(baseIndex + currentTabIndex, newTab, layer->name());
			addTabCloseButton(newTab->page());
			setTabTextColour(newTab->page(), QColor(0, 81, 0));
			setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_modulelayer.svg"));
		}

		++currentTabIndex;
	}
	baseIndex += dissolve.nProcessingLayers();
}


// Remove tab containing the specified page widget
void MainTabsWidget::removeByPage(QWidget* page)
{
	// Delete the tab from the tabwidget first - find its index (based on the page widget pointer) and remove that
	int indexToRemove = indexOf(page);
	if (indexToRemove == -1) printf("Couldn't remove tab since its page widget (%p) could not be found.\n", page);
	else removeTab(indexToRemove);

	// Now delete the tab from its list - this will delete the actual page widget
	bool updateAll = false;
	if (speciesTab(page))
	{
		allTabs_.remove(speciesTab(page));
		speciesTabs_.remove(speciesTab(page));
		updateAll = true;
	}
	else if (configurationTab(page))
	{
		allTabs_.remove(configurationTab(page));
		configurationTabs_.remove(configurationTab(page));
		updateAll = true;
	}
	else if (processingLayerTab(page))
	{
		allTabs_.remove(processingLayerTab(page));
		processingLayerTabs_.remove(processingLayerTab(page));
		updateAll = true;
	}
	else if (moduleTab(page))
	{
		allTabs_.remove(moduleTab(page));
		moduleTabs_.remove(moduleTab(page));
	}
	else if (workspaceTab(page))
	{
		allTabs_.remove(workspaceTab(page));
		workspaceTabs_.remove(workspaceTab(page));
	}
	else printf("Couldn't remove tab %p as it could not be found in any list.\n", page);

	if (updateAll)
	{
		emit(dataModified());
		updateAllTabs();
	}
}

// Create / go to Module tab for specified Module, provided it has a Module control widget
MainTab* MainTabsWidget::addModuleTab(DissolveWindow* dissolveWindow, Module* module)
{
	// Does a tab for this Module already exist
	ModuleTab* tab = moduleTab(module);
	if (!tab)
	{
		// Need to create a new ModuleTab
		tab = new ModuleTab(dissolveWindow, dissolveWindow->dissolve(), this, module->uniqueName(), module);
		moduleTabs_.own(tab);
		allTabs_.append(tab);
		addTab(tab, module->uniqueName());
		addTabCloseButton(tab);
		setTabIcon(tab->page(), QIcon(ModuleBlock::modulePixmap(module)));

		// If we are currently running, disable the necessary controls in widget
		if (dissolveWindow->dissolveState() == DissolveWindow::RunningState) tab->disableSensitiveControls();
	}

	setCurrentTab(tab);

	return tab;
}

// Remove the ModuleTab for the specifeid Module, if it exists
void MainTabsWidget::removeModuleTab(Module* module)
{
	ModuleTab* tab = moduleTab(module);
	if (!tab) return;

	removeByPage(tab->page());
}

// Add on a new workspace tab
MainTab* MainTabsWidget::addWorkspaceTab(DissolveWindow* dissolveWindow, const char* title)
{
	// Check that a tab with this title doesn't already exist
	MainTab* tab = findTab(title);
	if (!tab)
	{
		WorkspaceTab* newWorkspace = new WorkspaceTab(dissolveWindow, dissolveWindow->dissolve(), this, title);
		workspaceTabs_.own(newWorkspace);
		allTabs_.append(newWorkspace);

		// Add the new tab directly in to our tabs - it will not be managed in reconcileTabs().
		addTab(newWorkspace, title);
		addTabCloseButton(newWorkspace);
		setTabIcon(newWorkspace->page(), QIcon(":/tabs/icons/tabs_workspace.svg"));

		return newWorkspace;
	}
	else Messenger::printVerbose("Tab '%s' already exists, so returning that instead...\n", title);

	return tab;
}

/*
 * Display
 */

// Return current tab
MainTab* MainTabsWidget::currentTab() const
{
	if (currentWidget() == NULL) return NULL;

	// Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets are derived
	MainTab* currentTab = dynamic_cast<MainTab*>(currentWidget());
	if (!currentTab)
	{
		Messenger::print("Can't cast current tab (index %i) into a MainTab.\n", currentIndex());
		return NULL;
	}

	return currentTab;
}

// Make specified tab the current one
void MainTabsWidget::setCurrentTab(MainTab* tab)
{
	setCurrentWidget(tab->page());
}

// Make specified tab the current one (by index)
void MainTabsWidget::setCurrentTab(int tabIndex)
{
	setCurrentIndex(tabIndex);
}

// Make specified Species tab the current one
void MainTabsWidget::setCurrentTab(Species* species)
{
	if (!species) return;

	for (SpeciesTab* tab = speciesTabs_.first(); tab != NULL; tab = tab->next()) if (tab->species() == species)
	{
		setCurrentWidget(tab->page());
		return;
	}

	Messenger::error("Can't display SpeciesTab for Species '%s' as it doesn't exist.\n", species->name());
}

// Make specified Configuration tab the current one
void MainTabsWidget::setCurrentTab(Configuration* cfg)
{
	if (!cfg) return;

	for (ConfigurationTab* tab = configurationTabs_.first(); tab != NULL; tab = tab->next()) if (tab->configuration() == cfg)
	{
		setCurrentWidget(tab->page());
		return;
	}

	Messenger::error("Can't display ConfigurationTab for Configuration '%s' as it doesn't exist.\n", cfg->name());
}

// Make specified processing layer tab the current one
void MainTabsWidget::setCurrentTab(ModuleLayer* layer)
{
	if (!layer) return;

	for (LayerTab* tab = processingLayerTabs_.first(); tab != NULL; tab = tab->next()) if (tab->moduleLayer() == layer)
	{
		setCurrentWidget(tab->page());
		return;
	}

	Messenger::error("Can't display LayerTab for processing layer '%s' as it doesn't exist.\n", layer->name());
}

/*
 * Update
 */

// Update all tabs
void MainTabsWidget::updateAllTabs()
{
	for (MainTab* tab : allTabs_) tab->updateControls();
}

// Update all Species tabs
void MainTabsWidget::updateSpeciesTabs()
{
	ListIterator<SpeciesTab> tabIterator(speciesTabs_);
	while (SpeciesTab* tab = tabIterator.iterate()) tab->updateControls();
}

// Disable sensitive controls in all tabs
void MainTabsWidget::disableSensitiveControls()
{
	for (MainTab* tab : allTabs_) tab->disableSensitiveControls();

	// Disable tab close buttons
	RefDataListIterator<QToolButton,QWidget*> buttonIterator(closeButtons_);
	while (QToolButton* button = buttonIterator.iterate()) button->setDisabled(true);
}

// Enable sensitive controls in all tabs
void MainTabsWidget::enableSensitiveControls()
{
	for (MainTab* tab : allTabs_) tab->enableSensitiveControls();

	// Enable tab close buttons
	RefDataListIterator<QToolButton,QWidget*> buttonIterator(closeButtons_);
	while (QToolButton* button = buttonIterator.iterate()) button->setEnabled(true);
}

/*
 * Tab Styling
 */

// Set text colour for tab with specified page widget
void MainTabsWidget::setTabTextColour(QWidget* pageWidget, QColor colour)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("MainTabsWidget::setTabTextColour - Failed to find tab containing widget %p.\n", pageWidget);
		return;
	}

	// Set the style via the tab bar
	mainTabsBar_->setTabTextColor(tabIndex, colour);
}

// Set icon for tab with specified page widget
void MainTabsWidget::setTabIcon(QWidget* pageWidget, QIcon icon)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("MainTabsWidget::setTabIcon - Failed to find tab containing widget %p.\n", pageWidget);
		return;
	}

	// Set the style via the tab bar
	mainTabsBar_->setTabIcon(tabIndex, icon);
}

// Add close button to specified tab
QToolButton* MainTabsWidget::addTabCloseButton(QWidget* pageWidget)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("MainTabsWidget::addTabCloseButton - Failed to find tab containing widget %p.\n", pageWidget);
		return NULL;
	}

	// Create a suitable tool button for the tab
	QToolButton* closeButton = new QToolButton;
	closeButton->setIcon(QIcon(":/general/icons/general_cross.svg"));
	closeButton->setIconSize(QSize(10,10));
	closeButton->setAutoRaise(true);
	mainTabsBar_->setTabButton(tabIndex, QTabBar::RightSide	, closeButton);
	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(tabCloseButtonClicked(bool)));
	closeButtons_.append(closeButton, pageWidget);

	return closeButton;
}

/*
 * Widget Functions
 */

// Tab close button clicked
void MainTabsWidget::tabCloseButtonClicked(bool checked)
{
	// Find the close button that sent the signal in our buttons reflist
	QToolButton* toolButton = dynamic_cast<QToolButton*>(sender());
	if (!toolButton) return;

	RefDataItem<QToolButton,QWidget*>* item = closeButtons_.contains(toolButton);
	if (item)
	{
		// Find the tab containing the page widget (stored as the RefListItem's data)
		int tabIndex = indexOf(item->data());
		if (tabIndex == -1)
		{
			Messenger::error("MainTabsWidget::tabCloseButtonClicked - Failed to find tab containing widget %p.\n", item->data());
			return;
		}

		// Get the relevant widget (as a MainTab)
		MainTab* tab = dynamic_cast<MainTab*>(widget(tabIndex));
		if (!tab) return;
		MainTab::TabType tabType = tab->type();

		// Check whether the tab can / should be closed
		if (!tab->canClose()) return;

		// Grab the pointer to the page widget before we delete the button item
		QWidget* page = item->data();

		// Remove the button item
		closeButtons_.remove(item);

		// Delete the tab (referenced by its page widget)
		removeByPage(page);

		// Emit data modified signal dependent on tab type
		if ((tabType != MainTab::ModuleTabType) && (tabType != MainTab::WorkspaceTabType)) emit(dataModified());
	}
	else printf("Tabs received a close event from an unknown button...\n");
}

// Tab bar double-clicked
void MainTabsWidget::tabBarDoubleClicked(int index)
{
	if (index == -1) return;

	// Get the relevant widget (as a MainTab)
	MainTab* tab = dynamic_cast<MainTab*>(widget(index));
	if (!tab) return;

	// Call the rename function in the tab
	tab->rename();
}

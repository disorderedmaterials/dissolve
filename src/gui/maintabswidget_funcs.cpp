// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "gui/charts/moduleblock.h"
#include "gui/gui.h"
#include "gui/maintab.h"
#include "gui/maintabsbar.hui"
#include "gui/maintabswidget.hui"
#include "main/dissolve.h"
#include <QToolButton>
#include <memory>

MainTabsWidget::MainTabsWidget(QWidget *parent) : QTabWidget(parent)
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
    processingLayerTabs_.clear();
    configurationTabs_.clear();
    speciesTabs_.clear();
    allTabs_.clear();
}

/*
 * Tab Data
 */

// Return reference list of all current tabs
const std::vector<std::shared_ptr<MainTab>> &MainTabsWidget::allTabs() const { return allTabs_; }

// Return currently-selected Species (if a SpeciesTab is the current one)
Species *MainTabsWidget::currentSpecies() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Species)
        return nullptr;

    auto speciesTab = std::dynamic_pointer_cast<SpeciesTab>(tab);
    return (speciesTab ? speciesTab->species() : nullptr);
}

// Return currently-selected Configuration (if a ConfigurationTab is the current one)
Configuration *MainTabsWidget::currentConfiguration() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Configuration)
        return nullptr;

    auto configurationTab = std::dynamic_pointer_cast<ConfigurationTab>(tab);
    return (configurationTab ? configurationTab->configuration() : nullptr);
}

// Return currently-selected ModuleLayer (if a LayerTab is the current one)
ModuleLayer *MainTabsWidget::currentLayer() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Layer)
        return nullptr;

    auto layerTab = std::dynamic_pointer_cast<LayerTab>(tab);
    return (layerTab ? layerTab->moduleLayer() : nullptr);
}

// Find SpeciesTab containing specified page widget
std::shared_ptr<SpeciesTab> MainTabsWidget::speciesTab(QWidget *page)
{
    for (auto tab : speciesTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find ConfigurationTab containing specified page widget
std::shared_ptr<ConfigurationTab> MainTabsWidget::configurationTab(QWidget *page)
{
    for (auto tab : configurationTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find LayerTab containing specified page widget
std::shared_ptr<LayerTab> MainTabsWidget::processingLayerTab(QWidget *page)
{
    for (auto tab : processingLayerTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find WorkspaceTab containing specified page widget
std::shared_ptr<WorkspaceTab> MainTabsWidget::workspaceTab(QWidget *page)
{
    for (auto tab : workspaceTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find tab with title specified
std::shared_ptr<MainTab> MainTabsWidget::findTab(const QString title)
{
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(), [&title](auto tab) { return tab->title() == title; });
    return result == allTabs_.end() ? nullptr : *result;
}

// Find tab with specified page widget
std::shared_ptr<MainTab> MainTabsWidget::findTab(QWidget *page)
{
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(), [&page](auto tab) { return tab->page() == page; });
    return result == allTabs_.end() ? nullptr : *result;
}

// Generate unique tab name with base name provided
const QString MainTabsWidget::uniqueTabName(const QString base)
{
    static QString uniqueName;
    QString baseName = base;
    uniqueName = baseName;
    auto suffix = 0;

    // Must always have a baseName
    if (baseName.isEmpty())
        baseName = "Unnamed";

    // Find an unused name starting with the baseName provided
    while (findTab(uniqueName))
        uniqueName = QStringLiteral("%1%2").arg(baseName, ++suffix);

    return uniqueName;
}

/*
 * Tab Management
 */

// Add core tabs
void MainTabsWidget::addCoreTabs(DissolveWindow *dissolveWindow)
{
    // Forcefield
    forcefieldTab_ = std::make_shared<ForcefieldTab>(dissolveWindow, dissolveWindow->dissolve(), this, "Forcefield");
    addTab(forcefieldTab_->page(), "Forcefield");
    setTabIcon(forcefieldTab_->page(), QIcon(":/tabs/icons/tabs_ff.svg"));

    allTabs_.push_back(forcefieldTab_);
}

// Remove tabs related to the current data
void MainTabsWidget::clearTabs()
{
    // Empty our list of close button references
    closeButtons_.clear();

    // Removal of the tab and widget will be handled by the class destructors
    workspaceTabs_.clear();
    processingLayerTabs_.clear();
    configurationTabs_.clear();
    speciesTabs_.clear();
    forcefieldTab_ = nullptr;

    allTabs_.clear();
}

// Reconcile tabs, making them consistent with the provided data
void MainTabsWidget::reconcileTabs(DissolveWindow *dissolveWindow)
{
    auto &dissolve = dissolveWindow->dissolve();

    // Species - Global tab indices run from 1 (first tab after ForcefieldTab) to 1+nSpecies
    auto currentTabIndex = 0;
    auto baseIndex = 1;
    for (const auto &sp : dissolve.species())
    {
        // Loop over existing tabs
        while (currentTabIndex < speciesTabs_.size())
        {
            // If the existing tab is displaying the current Species already, then we can move on.
            // Otherwise, delete it.
            if (speciesTabs_[currentTabIndex]->species() == sp.get())
                break;
            else
            {
                allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), speciesTabs_[currentTabIndex]));
                speciesTabs_.erase(speciesTabs_.begin() + currentTabIndex);
            }
        }

        // If the current tab index is (now) out of range, add a new one
        if (currentTabIndex == speciesTabs_.size())
        {
            QString tabTitle = QString::fromStdString(std::string(sp->name()));
            speciesTabs_.push_back(std::make_shared<SpeciesTab>(dissolveWindow, dissolve, this, tabTitle, sp.get()));

            auto newTab = speciesTabs_.back();
            allTabs_.push_back(newTab);
            insertTab(baseIndex + currentTabIndex, newTab, tabTitle);
            addTabCloseButton(newTab->page());
            setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_species.svg"));
        }

        ++currentTabIndex;
    }
    baseIndex += dissolve.nSpecies();

    // Configurations - Global tab indices run from 1+nSpecies (first tab after last Species) to 1+nSpecies+nConfigurations
    currentTabIndex = 0;
    for (auto &cfg : dissolve.configurations())
    {
        // Loop over existing tabs
        while (currentTabIndex < configurationTabs_.size())
        {
            // If the existing tab is displaying the current Configuration already, then we can move on. Otherwise,
            // delete it
            if (configurationTabs_[currentTabIndex]->configuration() == cfg.get())
                break;
            else
            {
                allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), configurationTabs_[currentTabIndex]));
                configurationTabs_.erase(configurationTabs_.begin() + currentTabIndex);
            }
        }

        // If the current tab index is (now) out of range, add a new one
        if (currentTabIndex == configurationTabs_.size())
        {
            QString tabTitle = QString::fromStdString(std::string(cfg->name()));
            auto newTab = std::make_shared<ConfigurationTab>(dissolveWindow, dissolve, this, tabTitle, cfg.get());
            configurationTabs_.push_back(newTab);
            allTabs_.push_back(newTab);
            insertTab(baseIndex + currentTabIndex, newTab, tabTitle);
            addTabCloseButton(newTab->page());
            setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_configuration.svg"));
        }

        ++currentTabIndex;
    }
    baseIndex += dissolve.nConfigurations();

    // Processing Layers - Global tab indices run from 1+nSpecies+nConfigurations (first tab after last Configuration) to
    // 1+nSpecies+nConfigurations+nProcessingLayers
    ListIterator<ModuleLayer> processingLayerIterator(dissolve.processingLayers());
    currentTabIndex = 0;
    while (ModuleLayer *layer = processingLayerIterator.iterate())
    {
        // Loop over existing tabs
        while (currentTabIndex < processingLayerTabs_.size())
        {
            // If the existing tab is displaying the current ModuleLayer already, then we can move on. Otherwise,
            // delete it
            if (processingLayerTabs_[currentTabIndex]->moduleLayer() == layer)
                break;
            else
            {
                allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), processingLayerTabs_[currentTabIndex]));
                processingLayerTabs_.erase(processingLayerTabs_.begin() + currentTabIndex);
            }
        }

        // If the current tab index is (now) out of range, add a new one
        if (currentTabIndex == processingLayerTabs_.size())
        {
            QString tabTitle = QString::fromStdString(std::string(layer->name()));
            auto newTab = std::make_shared<LayerTab>(dissolveWindow, dissolve, this, tabTitle, layer);
            processingLayerTabs_.push_back(newTab);
            allTabs_.push_back(newTab);
            insertTab(baseIndex + currentTabIndex, newTab, tabTitle);
            addTabCloseButton(newTab->page());
            if (layer->enabled())
                setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_modulelayer.svg"));
            else
                setTabIcon(newTab->page(), QIcon(":/tabs/icons/tabs_modulelayer_disabled.svg"));
        }

        ++currentTabIndex;
    }
    baseIndex += dissolve.nProcessingLayers();
}

// Remove tab containing the specified page widget
void MainTabsWidget::removeByPage(QWidget *page)
{
    // Delete the tab from the tabwidget first - find its index (based on the page widget pointer) and remove that
    auto indexToRemove = indexOf(page);
    if (indexToRemove)
        removeTab(indexToRemove);

    // Now delete the tab from its list - this will delete the actual page widget
    auto updateAll = false;
    if (speciesTab(page))
    {
        allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), speciesTab(page)));
        speciesTabs_.erase(std::remove(speciesTabs_.begin(), speciesTabs_.end(), speciesTab(page)));
        updateAll = true;
    }
    else if (configurationTab(page))
    {
        allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), configurationTab(page)));
        configurationTabs_.erase(std::remove(configurationTabs_.begin(), configurationTabs_.end(), configurationTab(page)));
        updateAll = true;
    }
    else if (processingLayerTab(page))
    {
        allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), processingLayerTab(page)));
        processingLayerTabs_.erase(
            std::remove(processingLayerTabs_.begin(), processingLayerTabs_.end(), processingLayerTab(page)));
        updateAll = true;
    }
    else if (workspaceTab(page))
    {
        allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), workspaceTab(page)));
        workspaceTabs_.erase(std::remove(workspaceTabs_.begin(), workspaceTabs_.end(), workspaceTab(page)));
    }

    if (updateAll)
    {
        emit(dataModified());
        updateAllTabs();
    }
}

// Add on a new workspace tab
std::shared_ptr<MainTab> MainTabsWidget::addWorkspaceTab(DissolveWindow *dissolveWindow, const QString title)
{
    // Check that a tab with this title doesn't already exist
    auto tab = findTab(title);
    if (!tab)
    {
        auto newWorkspace = std::make_shared<WorkspaceTab>(dissolveWindow, dissolveWindow->dissolve(), this, title);
        workspaceTabs_.push_back(newWorkspace);
        allTabs_.push_back(newWorkspace);

        // Add the new tab directly in to our tabs - it will not be managed in reconcileTabs().
        addTab(newWorkspace.get(), title);
        addTabCloseButton(newWorkspace.get());
        setTabIcon(newWorkspace->page(), QIcon(":/tabs/icons/tabs_workspace.svg"));

        return newWorkspace;
    }
    else
        Messenger::printVerbose("Tab '{}' already exists, so returning that instead...\n", qPrintable(title));

    return tab;
}

/*
 * Display
 */

// Return current tab
std::shared_ptr<MainTab> MainTabsWidget::currentTab() const
{
    if (allTabs_.empty() || currentWidget() == nullptr)
        return nullptr;

    // Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets
    // are derived
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(),
                               [this](auto tab) { return tab.get() == dynamic_cast<MainTab *>(currentWidget()); });
    if (result == allTabs_.end())
    {
        Messenger::print("Can't cast current tab (index {}) into a MainTab.\n", currentIndex());
        return nullptr;
    }

    return *result;
}

// Make specified tab the current one
void MainTabsWidget::setCurrentTab(std::shared_ptr<MainTab> tab) { setCurrentWidget(tab->page()); }

// Make specified tab the current one (by index)
void MainTabsWidget::setCurrentTab(int tabIndex) { setCurrentIndex(tabIndex); }

// Make specified Species tab the current one
void MainTabsWidget::setCurrentTab(Species *species)
{
    if (!species)
        return;

    for (auto &tab : speciesTabs_)
        if (tab->species() == species)
        {
            setCurrentWidget(tab->page());
            return;
        }

    Messenger::error("Can't display SpeciesTab for Species '{}' as it doesn't exist.\n", species->name());
}

// Make specified Configuration tab the current one
void MainTabsWidget::setCurrentTab(Configuration *cfg)
{
    if (!cfg)
        return;

    for (auto tab : configurationTabs_)
        if (tab->configuration() == cfg)
        {
            setCurrentWidget(tab->page());
            return;
        }

    Messenger::error("Can't display ConfigurationTab for Configuration '{}' as it doesn't exist.\n", cfg->name());
}

// Make specified processing layer tab the current one
void MainTabsWidget::setCurrentTab(ModuleLayer *layer)
{
    if (!layer)
        return;

    for (auto tab : processingLayerTabs_)
        if (tab->moduleLayer() == layer)
        {
            setCurrentWidget(tab->page());
            return;
        }

    Messenger::error("Can't display LayerTab for processing layer '{}' as it doesn't exist.\n", layer->name());
}

/*
 * Update
 */

// Update all tabs
void MainTabsWidget::updateAllTabs()
{
    for (auto tab : allTabs_)
        tab->updateControls();
}

// Update all Species tabs
void MainTabsWidget::updateSpeciesTabs()
{
    for (auto tab : speciesTabs_)
        tab->updateControls();
}

// Disable sensitive controls in all tabs
void MainTabsWidget::disableSensitiveControls()
{
    for (auto tab : allTabs_)
        tab->disableSensitiveControls();

    // Disable tab close buttons
    for (auto &[button, page] : closeButtons_)
        button->setDisabled(true);
}

// Enable sensitive controls in all tabs
void MainTabsWidget::enableSensitiveControls()
{
    for (auto tab : allTabs_)
        tab->enableSensitiveControls();

    // Enable tab close buttons
    for (auto &[button, page] : closeButtons_)
        button->setEnabled(true);
}

/*
 * Tab Styling
 */

// Set text colour for tab with specified page widget
void MainTabsWidget::setTabTextColour(QWidget *pageWidget, QColor colour)
{
    // Find the tab containing the specified page
    auto tabIndex = indexOf(pageWidget);
    if (tabIndex == -1)
        return;

    // Set the style via the tab bar
    mainTabsBar_->setTabTextColor(tabIndex, colour);
}

// Set icon for tab with specified page widget
void MainTabsWidget::setTabIcon(QWidget *pageWidget, QIcon icon)
{
    // Find the tab containing the specified page
    auto tabIndex = indexOf(pageWidget);
    if (tabIndex == -1)
        return;

    // Set the style via the tab bar
    mainTabsBar_->setTabIcon(tabIndex, icon);
}

// Add close button to specified tab
QToolButton *MainTabsWidget::addTabCloseButton(QWidget *pageWidget)
{
    // Find the tab containing the specified page
    auto tabIndex = indexOf(pageWidget);
    if (tabIndex == -1)
        return nullptr;

    // Create a suitable tool button for the tab
    QToolButton *closeButton = new QToolButton;
    closeButton->setIcon(QIcon(":/general/icons/general_cross.svg"));
    closeButton->setIconSize(QSize(10, 10));
    closeButton->setAutoRaise(true);
    mainTabsBar_->setTabButton(tabIndex, QTabBar::RightSide, closeButton);
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(tabCloseButtonClicked(bool)));
    closeButtons_.emplace_back(closeButton, pageWidget);

    return closeButton;
}

/*
 * Widget Functions
 */

// Tab close button clicked
void MainTabsWidget::tabCloseButtonClicked(bool checked)
{
    // Find the close button that sent the signal in our buttons reflist
    QToolButton *toolButton = dynamic_cast<QToolButton *>(sender());
    if (!toolButton)
        return;

    auto it = std::find_if(closeButtons_.begin(), closeButtons_.end(),
                           [toolButton](auto &term) { return std::get<QToolButton *>(term) == toolButton; });
    if (it != closeButtons_.end())
    {
        auto [button, page] = *it;
        // Find the tab containing the page widget (stored as the RefListItem's data)
        auto tabIndex = indexOf(page);
        if (tabIndex == -1)
            return;

        // Get the relevant widget (as a MainTab)
        MainTab *tab = dynamic_cast<MainTab *>(widget(tabIndex));
        if (!tab)
            return;
        MainTab::TabType tabType = tab->type();

        // Check whether the tab can / should be closed
        if (!tab->canClose())
            return;

        // Remove the button item
        closeButtons_.erase(it);

        // Delete the tab (referenced by its page widget)
        removeByPage(page);

        // Emit data modified signal dependent on tab type
        if (tabType != MainTab::TabType::Workspace)
            emit(dataModified());
    }
}

// Tab bar double-clicked
void MainTabsWidget::tabBarDoubleClicked(int index)
{
    if (index == -1)
        return;

    // Get the relevant widget (as a MainTab)
    MainTab *tab = dynamic_cast<MainTab *>(widget(index));
    if (!tab)
        return;

    // Call the rename function in the tab
    tab->rename();
}

int MainTabsWidget::insertTab(int index, std::shared_ptr<QWidget> widget, const QString &title)
{
    return QTabWidget::insertTab(index, dynamic_cast<QWidget *>(widget.get()), title);
}

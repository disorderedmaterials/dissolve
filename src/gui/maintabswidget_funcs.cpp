// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/messenger.h"
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
    processingLayerTabs_.clear();
    configurationTabs_.clear();
    speciesTabs_.clear();
    allTabs_.clear();
}

/*
 * UI
 */

// Set-up widget
void MainTabsWidget::setUp(DissolveWindow *dissolveWindow)
{
    forcefieldTab_ = new ForcefieldTab(dissolveWindow, dissolveWindow->dissolve(), this, "Forcefield");
    messagesTab_ = new MessagesTab(dissolveWindow, dissolveWindow->dissolve(), this, "Messages");
}

/*
 * Tab Data
 */

// Return reference list of all current tabs
const std::vector<MainTab *> &MainTabsWidget::allTabs() const { return allTabs_; }

// Return currently-selected Species (if a SpeciesTab is the current one)
Species *MainTabsWidget::currentSpecies() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Species)
        return nullptr;

    auto speciesTab = dynamic_cast<SpeciesTab *>(tab);
    return (speciesTab ? speciesTab->species() : nullptr);
}

// Return currently-selected Configuration (if a ConfigurationTab is the current one)
Configuration *MainTabsWidget::currentConfiguration() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Configuration)
        return nullptr;

    auto configurationTab = dynamic_cast<ConfigurationTab *>(tab);
    return (configurationTab ? configurationTab->configuration() : nullptr);
}

// Return currently-selected ModuleLayer (if a LayerTab is the current one)
ModuleLayer *MainTabsWidget::currentLayer() const
{
    // Get the currently-selected tab, and make sure it's a SpeciesTab
    auto tab = currentTab();
    if (tab->type() != MainTab::TabType::Layer)
        return nullptr;

    auto layerTab = dynamic_cast<LayerTab *>(tab);
    return (layerTab ? layerTab->moduleLayer() : nullptr);
}

// Return MessagesTab
MessagesTab *MainTabsWidget::messagesTab() { return messagesTab_.data(); }

// Return the ForcefieldTab
ForcefieldTab *MainTabsWidget::forcefieldTab() { return forcefieldTab_.data(); }

// Find SpeciesTab containing specified page widget
QPointer<SpeciesTab> MainTabsWidget::speciesTab(QWidget *page)
{
    for (auto tab : speciesTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find ConfigurationTab containing specified page widget
QPointer<ConfigurationTab> MainTabsWidget::configurationTab(QWidget *page)
{
    for (auto tab : configurationTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find LayerTab containing specified page widget
QPointer<LayerTab> MainTabsWidget::processingLayerTab(QWidget *page)
{
    for (auto tab : processingLayerTabs_)
        if (tab->page() == page)
            return tab;

    return nullptr;
}

// Find tab with title specified
MainTab *MainTabsWidget::findTab(const QString title)
{
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(), [&title](auto tab) { return tab->title() == title; });
    return result == allTabs_.end() ? nullptr : *result;
}

// Find tab displaying specified page
MainTab *MainTabsWidget::findTab(const QWidget *page)
{
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(), [page](auto tab) { return tab->page() == page; });
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

// Remove all tabs, including permanent tabs
void MainTabsWidget::clearTabs()
{
    // Empty our list of close button references
    closeButtons_.clear();

    // Removal of the tab and widget will be handled by the class destructors
    processingLayerTabs_.clear();
    configurationTabs_.clear();
    speciesTabs_.clear();

    // Clear all tab references, and then re-add the permanent ones
    allTabs_.clear();
    clear();

    // -- Messages
    addTab(messagesTab_->page(), "Messages");
    setTabIcon(messagesTab_->page(), QIcon(":/tabs/icons/tabs_messages.svg"));
    allTabs_.push_back(messagesTab_.data());
    // -- Forcefield
    addTab(forcefieldTab_->page(), "Forcefield");
    setTabIcon(forcefieldTab_->page(), QIcon(":/tabs/icons/tabs_ff.svg"));
    allTabs_.push_back(forcefieldTab_.data());
}

// Reconcile tabs, making them consistent with the provided data
void MainTabsWidget::reconcileTabs(DissolveWindow *dissolveWindow)
{
    auto &dissolve = dissolveWindow->dissolve();

    // Species - Global tab indices run from 1 (first tab after ForcefieldTab) to 1+nSpecies
    auto currentTabIndex = 0;
    auto baseIndex = 2;
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
            auto spTab = speciesTabs_.emplace_back(new SpeciesTab(dissolveWindow, dissolve, this, tabTitle, sp.get()));

            allTabs_.emplace_back(spTab.data());
            insertTab(baseIndex + currentTabIndex, spTab.data(), tabTitle);
            addTabCloseButton(spTab->page());
            setTabIcon(spTab->page(), QIcon(":/tabs/icons/tabs_species.svg"));
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
            // If the existing tab is displaying the current Configuration already, then we can move on. Otherwise,delete it
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
            auto cfgTab =
                configurationTabs_.emplace_back(new ConfigurationTab(dissolveWindow, dissolve, this, tabTitle, cfg.get()));

            allTabs_.push_back(cfgTab.data());
            insertTab(baseIndex + currentTabIndex, cfgTab.data(), tabTitle);
            addTabCloseButton(cfgTab->page());
            setTabIcon(cfgTab->page(), QIcon(":/tabs/icons/tabs_configuration.svg"));
        }

        ++currentTabIndex;
    }
    baseIndex += dissolve.nConfigurations();

    // Processing Layers - Global tab indices run from 1+nSpecies+nConfigurations (first tab after last Configuration) to
    // 1+nSpecies+nConfigurations+nProcessingLayers
    currentTabIndex = 0;
    for (const auto &layer : dissolve.processingLayers())
    {
        // Loop over existing tabs
        while (currentTabIndex < processingLayerTabs_.size())
        {
            // If the existing tab is displaying the current ModuleLayer already, then we can move on. Otherwise delete it.
            if (processingLayerTabs_[currentTabIndex]->moduleLayer() == layer.get())
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
            auto layerTab =
                processingLayerTabs_.emplace_back(new LayerTab(dissolveWindow, dissolve, this, tabTitle, layer.get()));

            allTabs_.push_back(layerTab.data());
            insertTab(baseIndex + currentTabIndex, layerTab.data(), tabTitle);
            addTabCloseButton(layerTab->page());
            if (layer->isEnabled())
                setTabIcon(layerTab->page(), QIcon(":/tabs/icons/tabs_layer.svg"));
            else
                setTabIcon(layerTab->page(), QIcon(":/tabs/icons/tabs_layer_disabled.svg"));
        }

        ++currentTabIndex;
    }
    baseIndex += dissolve.nProcessingLayers();
}

// Remove tab containing the specified page widget
void MainTabsWidget::removeByPage(QWidget *page)
{
    // Delete the tab from the widget first - find its index (based on the page widget pointer) and remove that
    auto indexToRemove = indexOf(page);
    if (indexToRemove)
        removeTab(indexToRemove);

    // Now delete the tab from our reference vectors
    auto *tab = findTab(page);
    allTabs_.erase(std::remove(allTabs_.begin(), allTabs_.end(), tab));
    switch (tab->type())
    {
        case (MainTab::TabType::Configuration):
            configurationTabs_.erase(std::remove(configurationTabs_.begin(), configurationTabs_.end(), configurationTab(page)));
            break;
        case (MainTab::TabType::Layer):
            processingLayerTabs_.erase(
                std::remove(processingLayerTabs_.begin(), processingLayerTabs_.end(), processingLayerTab(page)));
            break;
        case (MainTab::TabType::Species):
            speciesTabs_.erase(std::remove(speciesTabs_.begin(), speciesTabs_.end(), speciesTab(page)));
            break;
        default:
            break;
    }
}

/*
 * Display
 */

// Return current tab
MainTab *MainTabsWidget::currentTab() const
{
    if (allTabs_.empty() || currentWidget() == nullptr)
        return nullptr;

    // Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets
    // are derived
    auto result = std::find_if(allTabs_.begin(), allTabs_.end(),
                               [this](auto tab) { return tab == dynamic_cast<MainTab *>(currentWidget()); });
    if (result == allTabs_.end())
    {
        Messenger::print("Can't cast current tab (index {}) into a MainTab.\n", currentIndex());
        return nullptr;
    }

    return *result;
}

// Make specified tab the current one
void MainTabsWidget::setCurrentTab(const MainTab *tab) { setCurrentWidget(tab->page()); }

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

// Prevent editing in all tabs
void MainTabsWidget::preventEditing()
{
    for (auto tab : allTabs_)
        tab->preventEditing();

    // Disable tab close buttons
    for (auto &[button, page] : closeButtons_)
        button->setDisabled(true);
}

// Allow editing in all tabs
void MainTabsWidget::allowEditing()
{
    for (auto tab : allTabs_)
        tab->allowEditing();

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
    auto *closeButton = new QToolButton;
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
    auto *toolButton = dynamic_cast<QToolButton *>(sender());
    if (!toolButton)
        return;

    auto it = std::find_if(closeButtons_.begin(), closeButtons_.end(),
                           [toolButton](auto &term) { return std::get<QToolButton *>(term) == toolButton; });
    if (it == closeButtons_.end())
        return;

    // Find the tab containing the page widget
    auto [button, page] = *it;
    auto *tab = findTab(page);
    if (!tab || !tab->canClose())
        return;

    // Remove the button item
    closeButtons_.erase(it);

    // Tell the main GUI to do the clean-up
    emit(tabClosed(page));
}

// Tab bar double-clicked
void MainTabsWidget::tabBarDoubleClicked(int index)
{
    if (index == -1)
        return;

    // Get the relevant widget (as a MainTab)
    auto *tab = dynamic_cast<MainTab *>(widget(index));
    if (!tab)
        return;

    // Call the rename function in the tab
    tab->rename();
}

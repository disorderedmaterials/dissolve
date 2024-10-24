// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/configurationTab.h"
#include "gui/forcefieldTab.h"
#include "gui/layerTab.h"
#include "gui/messagesTab.h"
#include "gui/overviewTab.h"
#include "gui/speciesTab.h"
#include <QPointer>
#include <QTabWidget>
#include <memory>
#include <vector>

// Forward declarations
class Dissolve;
class MainTabsBar;
class QToolButton;

// MainTabsWidget
class MainTabsWidget : public QTabWidget
{
    Q_OBJECT

    public:
    MainTabsWidget(QWidget *parent);
    ~MainTabsWidget();

    /*
     * UI
     */
    private:
    // Tab bar
    MainTabsBar *mainTabsBar_;

    public:
    // Set-up widget
    void setUp(DissolveWindow *dissolveWindow);
    // Add in reference to DissolveWindow for all functions to access
    DissolveWindow *dissolveWindow_{nullptr};

    /*
     * Tab Data
     */
    private:
    // Reference list of all available tabs
    std::vector<MainTab *> allTabs_;
    // Messages tab
    QPointer<MessagesTab> messagesTab_;
    // Pointer to Forcefield tab
    QPointer<ForcefieldTab> forcefieldTab_;
    // OverviewTab Toggle
    static constexpr bool overview_flag = false;
    // Pointer to Overview tab
    QPointer<OverviewTab> overviewTab_;
    // List of Species tabs
    std::vector<QPointer<SpeciesTab>> speciesTabs_;
    // List of Configuration tabs
    std::vector<QPointer<ConfigurationTab>> configurationTabs_;
    // List of processing layer tabs
    std::vector<QPointer<LayerTab>> processingLayerTabs_;

    public:
    // Return reference list of all current tabs
    const std::vector<MainTab *> &allTabs() const;
    // Return currently-selected Species (if a SpeciesTab is the current one)
    Species *currentSpecies() const;
    // Return currently-selected Configuration (if a ConfigurationTab is the current one)
    Configuration *currentConfiguration() const;
    // Return currently-selected ModuleLayer (if a LayerTab is the current one)
    ModuleLayer *currentLayer() const;
    // Return MessagesTab
    MessagesTab *messagesTab();
    // Return the ForcefieldTab
    ForcefieldTab *forcefieldTab();
    // Find SpeciesTab containing specified page widget
    QPointer<SpeciesTab> speciesTab(QWidget *page);
    // Find ConfigurationTab containing specified page widget
    QPointer<ConfigurationTab> configurationTab(QWidget *page);
    // Find LayerTab containing specified page widget
    QPointer<LayerTab> processingLayerTab(QWidget *page);
    // Find tab displaying specified page
    MainTab *findTab(const QWidget *page);

    /*
     * Tab Management
     */
    public:
    // Remove all tabs, including permanent tabs
    void clearTabs();
    // Reconcile tabs, making them consistent with the provided data
    void reconcileTabs();
    // Remove tab containing the specified page widget
    void removeByPage(QWidget *page);

    /*
     * Display
     */
    public:
    // Return current tab
    MainTab *currentTab() const;
    // Make specified Species tab the current one
    void setCurrentTab(Species *species);
    void setCurrentTab(const Species *species);
    // Make specified Configuration tab the current one
    void setCurrentTab(Configuration *cfg);
    // Make specified processing layer tab the current one
    void setCurrentTab(ModuleLayer *layer);

    /*
     * Update
     */
    private:
    // Whether editing is currently enabled
    bool editingEnabled_{true};

    public:
    // Update all tabs
    void updateAllTabs();
    // Prevent editing in all tabs
    void preventEditing();
    // Allow editing in all tabs
    void allowEditing();

    /*
     * Tab Styling
     */
    public:
    // Set icon for tab with specified page widget
    void setTabIcon(QWidget *pageWidget, QIcon icon);
    // Add close button to specified tab
    QToolButton *addTabCloseButton(QWidget *pageWidget);

    /*
     * Widget Functions
     */
    private:
    // List of close buttons and their associated pageWidgets
    std::vector<std::tuple<QToolButton *, QWidget *>> closeButtons_;

    private Q_SLOTS:
    // Context menu requested
    void contextMenuRequested(const QPoint &pos);
    // Tab close button clicked
    void tabCloseButtonClicked(bool);
    // Tab double-clicked
    void tabDoubleClicked(int index);

    Q_SIGNALS:
    void dataModified();
    void tabClosed(QWidget *);
};

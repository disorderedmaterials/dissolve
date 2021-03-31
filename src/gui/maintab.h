// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "base/lock.h"
#include "module/module.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include <QString>

// Forward Declarations
class Dissolve;
class DissolveWindow;
class LineParser;
class MainTabsWidget;
class QLayout;
class QMdiArea;
class QWidget;

// Base Tab
class MainTab
{
    public:
    MainTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title, QWidget *page);
    virtual ~MainTab();
    // Tab Types
    enum TabType
    {
        ConfigurationTabType,
        ForcefieldTabType,
        LayerTabType,
        SpeciesTabType,
        WorkspaceTabType
    };
    // Return enum options for TabType
    static EnumOptions<MainTab::TabType> tabTypes();

    /*
     * Data
     */
    protected:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Pointer to main window
    DissolveWindow *dissolveWindow_;
    // MainTabsWidget in which this tab is contained
    MainTabsWidget *tabWidget_;
    // Page widget
    QWidget *page_;
    // Unique title (name) of tab
    QString title_;

    protected:
    // Raise suitable dialog for entering / checking new tab name
    virtual QString getNewTitle(bool &ok);

    public:
    // Return tab type
    virtual TabType type() const = 0;
    // Return page widget
    QWidget *page() const;
    // Return title of tab
    const QString title() const;

    /*
     * Management
     */
    protected:
    // Return whether the title of the tab can be changed
    virtual bool canChangeTitle() const;

    public:
    // Rename tab through suitable dialog / widget
    bool rename();
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    virtual bool canClose() const;

    /*
     * Update
     */
    protected:
    // Lock for widget refresh
    Lock refreshLock_;

    public:
    // Update controls in tab
    virtual void updateControls() = 0;
    // Disable sensitive controls within the tab
    virtual void disableSensitiveControls() = 0;
    // Enable sensitive controls within the tab
    virtual void enableSensitiveControls() = 0;
};

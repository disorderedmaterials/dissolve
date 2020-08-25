/*
    *** Main Tab
    *** src/gui/maintab.h
    Copyright T. Youngs 2012-2020

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
        ModuleTabType,
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

    /*
     * State
     */
    public:
    // Write widget state through specified LineParser
    virtual bool writeState(LineParser &parser) const = 0;
    // Read widget state through specified LineParser
    virtual bool readState(LineParser &parser, const CoreData &coreData) = 0;
};

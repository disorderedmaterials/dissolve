/*
    *** Workspace Tab
    *** src/gui/workspacetab.h
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

#include "gui/gizmo.h"
#include "gui/maintab.h"
#include "gui/ui_workspacetab.h"

// Forward Declarations
class TMdiArea;
class QMenu;

// Workspace Tab
class WorkspaceTab : public QWidget, public ListItem<WorkspaceTab>, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    WorkspaceTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title);
    ~WorkspaceTab();
    // Main form declaration
    Ui::WorkspaceTab ui;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok);
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const;

    /*
     * Update
     */
    protected:
    // Update controls in tab
    void updateControls();
    // Disable sensitive controls within tab
    void disableSensitiveControls();
    // Enable sensitive controls within tab
    void enableSensitiveControls();

    /*
     * Gizmo Management
     */
    private:
    // MDI area containing our Gizmos
    TMdiArea *mdiArea_;
    // List of current Gizmos in the MDI area
    List<Gizmo> gizmos_;

    private slots:
    // Remove Gizmo with specified unique name
    void removeGizmo(QString uniqueName);

    public:
    // Create Gizmo with specified type in this workspace
    Gizmo *createGizmo(QString type);

    /*
     * Context Menu
     */
    private slots:
    // Create Gizmo from context menu item
    void contextMenuAddGizmo(bool checked);

    public slots:
    // Custom context menu requested
    void showContextMenu(const QPoint &pos);

    /*
     * State
     */
    public:
    // Read widget state through specified LineParser
    bool readState(LineParser &parser, const CoreData &coreData);
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
};

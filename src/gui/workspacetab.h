// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
};

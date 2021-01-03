// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/getgizmonamedialog.h"
#include "gui/gui.h"
#include "templates/variantpointer.h"

/*
 * Private Slots
 */

void DissolveWindow::on_WorkspaceCreateEmptyAction_triggered(bool checked)
{
    MainTab *workspaceTab = ui_.MainTabs->addWorkspaceTab(this, "New Workspace");

    ui_.MainTabs->setCurrentTab(workspaceTab);
}

void DissolveWindow::on_WorkspaceRenameCurrentGizmoAction_triggered(bool checked)
{
    // Get current data from the action (it should be our target Gizmo)
    Gizmo *currentGizmo = VariantPointer<Gizmo>(ui_.WorkspaceRenameCurrentGizmoAction->data());
    if (!currentGizmo)
        return;

    // Get a new, valid name for the gizmo
    GetGizmoNameDialog nameDialog(this);
    if (nameDialog.get(currentGizmo, currentGizmo->uniqueName()))
    {
        // Rename the Gizmo and change the text in the menu item
        currentGizmo->setUniqueName(qPrintable(nameDialog.newName()));
        ui_.WorkspaceRenameCurrentGizmoAction->setText(QString("Rename '%1'...").arg(nameDialog.newName()));
    }
}

/*
 * Public Slots
 */

void DissolveWindow::currentWorkspaceGizmoChanged(QMdiSubWindow *gizmoWindow)
{
    Gizmo *currentGizmo = gizmoWindow ? Gizmo::find(gizmoWindow) : nullptr;
    if (!currentGizmo || !gizmoWindow)
    {
        ui_.WorkspaceRenameCurrentGizmoAction->setText("Rename Current Gizmo...");
        ui_.WorkspaceRenameCurrentGizmoAction->setEnabled(false);
        ui_.WorkspaceRenameCurrentGizmoAction->setData(VariantPointer<Gizmo>(nullptr));
        return;
    }

    // Set the text, status, and data of the menu item
    ui_.WorkspaceRenameCurrentGizmoAction->setText(QString("Rename '%1'...").arg(currentGizmo->uniqueName()));
    ui_.WorkspaceRenameCurrentGizmoAction->setEnabled(true);
    ui_.WorkspaceRenameCurrentGizmoAction->setData(VariantPointer<Gizmo>(currentGizmo));
}

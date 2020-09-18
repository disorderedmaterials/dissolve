/*
    *** Dissolve GUI - Workspace Menu Functions
    *** src/gui/menu_workspace.cpp
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

#include "gui/getgizmonamedialog.h"
#include "gui/gui.h"

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

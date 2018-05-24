/*
	*** Dissolve Main Window - Menu Functions
	*** src/gui/gui_menu.cpp
	Copyright T. Youngs 2012-2018

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
#include "gui/modulecontrolwidget.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QInputDialog>

/*
 * Main Menu
 */

void DissolveWindow::on_SessionOpenAction_triggered(bool checked)
{
}

void DissolveWindow::on_SessionSaveAction_triggered(bool checked)
{
	// Attempt to save the file (TEST)
	dissolve_.saveInput("shitty.txt");

	modified_ = false;

	updateStatus();
}

void DissolveWindow::on_SessionQuitAction_triggered(bool checked)
{
}

void DissolveWindow::on_WorkspaceAddNewAction_triggered(bool checked)
{
	// Add a new workspace
	bool ok;
	QString text = QInputDialog::getText(this, "New Workspace", "Enter the name of the new workspace", QLineEdit::Normal, "New Workspace", &ok);
	if (!ok || text.isEmpty()) return;

	MainTab* workspaceTab = addWorkspaceTab(qPrintable(text));

	setCurrentTab(workspaceTab);
}

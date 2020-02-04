/*
	*** Dissolve GUI - Start Page Functions
	*** src/gui/gui_start.cpp
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

#include "gui/gui.h"
#include "main/dissolve.h"

/*
 * Create
 */

void DissolveWindow::on_StartCreateNewButton_clicked(bool checked)
{
	ui_.FileNewAction->trigger();
}

void DissolveWindow::on_StartCreateFromTemplateButton_clicked(bool checked)
{
	ui_.FileNewFromTemplateAction->trigger();
}

/*
 * Open / Connect
 */

void DissolveWindow::on_StartOpenLocalButton_clicked(bool checked)
{
	ui_.FileOpenLocalAction->trigger();
}

void DissolveWindow::on_StartOpenRecentButton_clicked(bool checked)
{
	ui_.FileOpenRecentAction->trigger();
}

void DissolveWindow::on_StartConnectButton_clicked(bool checked)
{
	ui_.FileConnectAction->trigger();
}

/*
 * Help
 */

void DissolveWindow::on_StartOnlineManualButton_clicked(bool checked)
{
	ui_.HelpOnlineManualAction->trigger();
}

void DissolveWindow::on_StartOnlineTutorialsButton_clicked(bool checked)
{
	ui_.HelpOnlineTutorialsAction->trigger();
}

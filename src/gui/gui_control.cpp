/*
	*** Dissolve GUI - Control
	*** src/gui/gui_control.cpp
	Copyright T. Youngs 2012-2019

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

#include "main/dissolve.h"
#include "gui/gui.h"

// Return current state of Dissolve
DissolveWindow::DissolveState DissolveWindow::dissolveState() const
{
	return dissolveState_;
}

void DissolveWindow::on_ControlRunButton_clicked(bool checked)
{
	ui_.SimulationRunAction->trigger();
}

void DissolveWindow::on_ControlStepButton_clicked(bool checked)
{
	ui_.SimulationStepAction->trigger();
}

void DissolveWindow::on_ControlPauseButton_clicked(bool checked)
{
	ui_.SimulationPauseAction->trigger();
}

void DissolveWindow::on_ControlReloadButton_clicked(bool checked)
{
	// TODO
}

// Set widgets ready for the main code to be run
void DissolveWindow::setWidgetsForRun()
{
	// Disable / enable controls
	ui_.ControlRunButton->setEnabled(false);
	ui_.ControlStepButton->setEnabled(false);
	ui_.ControlPauseButton->setEnabled(true);

	// Disable sensitive controls in all tabs
	RefList<MainTab> tabs = allTabs();
	RefListIterator<MainTab> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->disableSensitiveControls();
}

// Set widgets after the main code has been run
void DissolveWindow::setWidgetsAfterRun()
{
	// Disable / enable controls
	ui_.ControlRunButton->setEnabled(true);
	ui_.ControlStepButton->setEnabled(true);
	ui_.ControlPauseButton->setEnabled(false);

	// Enable necessary controls in all tabs
	RefList<MainTab> tabs = allTabs();
	RefListIterator<MainTab> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->enableSensitiveControls();
}

// All iterations requested are complete
void DissolveWindow::iterationsComplete()
{
	setWidgetsAfterRun();
}

/*
	*** Dissolve GUI - Simulation Page - Control
	*** src/gui/gui_simulation_control.cpp
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

#include "main/dissolve.h"
#include "gui/gui.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/moduletab.h"
#include "gui/modulelayertab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"

// Return current state of Dissolve
DissolveWindow::DissolveState DissolveWindow::dissolveState() const
{
	return dissolveState_;
}

void DissolveWindow::on_ControlSetUpButton_clicked(bool checked)
{
	// Are we already set-up?
	if (dissolve_.isSetUp())
	{
		Messenger::print("Everything is already set-up and ready to go.\n");
		return;
	}

	dissolve_.setUp();

	updateControlsFrame();
}

void DissolveWindow::on_ControlRunButton_clicked(bool checked)
{
	ui.SimulationRunAction->trigger();
}

void DissolveWindow::on_ControlPauseButton_clicked(bool checked)
{
	ui.SimulationPauseAction->trigger();
}

void DissolveWindow::on_ControlReloadButton_clicked(bool checked)
{
}

// Set widgets ready for the main code to be run
void DissolveWindow::setWidgetsForRun()
{
	// Disable / enable controls
	ui.ControlRunButton->setEnabled(false);
	ui.ControlPauseButton->setEnabled(true);

	// Disable sensitive controls in all tabs
	RefList<MainTab,bool> tabs = allTabs();
	RefListIterator<MainTab,bool> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->disableSensitiveControls();
}

// Set widgets after the main code has been run
void DissolveWindow::setWidgetsAfterRun()
{
	// Disable / enable controls
	ui.ControlRunButton->setEnabled(true);
	ui.ControlPauseButton->setEnabled(false);

	// Enable necessary controls in all tabs
	RefList<MainTab,bool> tabs = allTabs();
	RefListIterator<MainTab,bool> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->enableSensitiveControls();
}

// All iterations requested are complete
void DissolveWindow::iterationsComplete()
{
	setWidgetsAfterRun();
}

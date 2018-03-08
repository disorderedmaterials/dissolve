/*
	*** dUQ Main Window - Control
	*** src/gui/gui_control.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/duq.h"
#include "gui/gui.h"

// Return current state of dUQ
DUQWindow::DUQState DUQWindow::duqState() const
{
	return duqState_;
}

void DUQWindow::on_ControlSetUpButton_clicked(bool checked)
{
	// Are we already set-up?
	if (duq_.isSetUp())
	{
		Messenger::print("Everything is already set-up and ready to go.\n");
		return;
	}

	duq_.setUp();

	updateStatus();
}

void DUQWindow::on_ControlRunButton_clicked(bool checked)
{
	// Make sure everything is set-up
	if ((!duq_.isSetUp()) && (!duq_.setUp())) return;

	updateStatus();

	// Prepare the GUI
	setWidgetsForRun();

	duqState_ = DUQWindow::RunningState;

	emit iterate(-1);
}

void DUQWindow::on_ControlStepButton_clicked(bool checked)
{
	// Make sure everything is set-up
	if ((!duq_.isSetUp()) && (!duq_.setUp())) return;

	updateStatus();

	// Prepare the GUI
	setWidgetsForRun();

	duqState_ = DUQWindow::RunningState;

	emit iterate(1);
}

void DUQWindow::on_ControlStepFiveButton_clicked(bool checked)
{
	// Make sure everything is set-up
	if ((!duq_.isSetUp()) && (!duq_.setUp())) return;

	updateStatus();

	// Prepare the GUI
	setWidgetsForRun();

	duqState_ = DUQWindow::RunningState;

	emit iterate(5);
}

void DUQWindow::on_ControlPauseButton_clicked(bool checked)
{
	duqState_ = DUQWindow::StoppedState;

	updateStatus();

	emit stopIterating();
}

void DUQWindow::on_ControlReloadButton_clicked(bool checked)
{
}

// Set widgets ready for the main code to be run
void DUQWindow::setWidgetsForRun()
{
	// Disable run and step buttons, and enable pause button
	ui.ControlRunButton->setEnabled(false);
	ui.ControlStepButton->setEnabled(false);
	ui.ControlStepFiveButton->setEnabled(false);
	ui.ControlPauseButton->setEnabled(true);

	// Disable sensitive controls in tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next) tab->disableSensitiveControls();
}

// Set widgets after the main code has been run
void DUQWindow::setWidgetsAfterRun()
{
	// Enable run and step buttons, and disable pause button
	ui.ControlRunButton->setEnabled(true);
	ui.ControlStepButton->setEnabled(true);
	ui.ControlStepFiveButton->setEnabled(true);
	ui.ControlPauseButton->setEnabled(false);

	// Enable necessary controls in tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next) tab->enableSensitiveControls();
}

// All iterations requested are complete
void DUQWindow::iterationsComplete()
{
	setWidgetsAfterRun();
}

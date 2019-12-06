/*
	*** Dissolve GUI - Simulation Menu Functions
	*** src/gui/menu_simulation.cpp
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

#include "gui/gui.h"
#include "gui/datamanagerdialog.h"
#include "main/dissolve.h"
#include <QInputDialog>

void DissolveWindow::on_SimulationRunAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(-1);
}

void DissolveWindow::on_SimulationStepAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(1);
}

void DissolveWindow::on_SimulationStepFiveAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(5);
}

void DissolveWindow::on_SimulationPauseAction_triggered(bool checked)
{
	// Set run icon button to the 'pausing' icon (it will be set back to normal by setWidgetsAfterRun())
	ui_.ControlRunButton->setIcon(QIcon(":/control/icons/control_waiting.svg"));

	// Send the signal to stop
	emit(stopIterating());

	// Disable the pause button
	ui_.ControlPauseButton->setEnabled(false);
}

void DissolveWindow::on_SimulationCreateSnapshotAction_triggered(bool checked)
{
	// First, get prefix name for snapshot
	bool ok;
	QString prefix = QInputDialog::getText(this, "Snapshot Prefix", "Enter the prefix for the snapshot");
}

void DissolveWindow::on_SimulationDataManagerAction_triggered(bool checked)
{
	DataManagerDialog dataManagerDialog(this, dissolve_);
	dataManagerDialog.exec();
}

void DissolveWindow::on_SimulationSetRandomSeedAction_triggered(bool checked)
{
	// Create an input dialog to get the new seed
	bool ok;
	dissolve_.seed();
	int newSeed = QInputDialog::getInt(this, "Set random seed", "Enter the new value of the random seed, or -1 to remove set value", dissolve_.seed(), -1, 2147483647, 1, &ok);

	if (!ok) return;

	// Set and initialise random seed
	dissolve_.setSeed(newSeed);

	if (dissolve_.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dissolve_.seed());
}

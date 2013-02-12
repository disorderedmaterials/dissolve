/*
	*** Main Window - Signal Handling
	*** src/gui/mainwindow_sigmals.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/mainwindow.h"

/*!
 * \brief Signal receiver from GuidUQ
 * \details This signal handler is set up to receive important updates from dUQ as it is running,
 * and update the GUI accordingly.
 */
void MainWindow::receiveDUQSignal(int signal, int data)
{
	printf("SIGN %i Current thread = %p\n", signal, QThread::currentThread());
	switch (signal)
	{
		case (DUQ::ConfigurationUpdatedSignal):
			simulationViewerConfiguration_ = dUQ_.checkPointConfiguration()->checkOut();
			dUQ_.checkPointConfiguration()->checkIn();
			ui.SimulationViewer->update();
			break;
		case (DUQ::EnergyUpdatedSignal):
			ui.SimulationEnergyGraph->refreshData();
			break;
		case (DUQ::PairCorrelationsUpdatedSignal):
			ui.SimulationSQGraph->refreshData();
			ui.SimulationFQGraph->refreshData();
			ui.SimulationPartialGRGraph->refreshData();
			ui.SimulationGRGraph->refreshData();
			break;
		case (DUQ::PairPotentialsUpdatedSignal):
			ui.SimulationPPGraph->refreshData();
			break;
		default:
			printf("GUI Error: Signal %i not handled in MainWindow::receiveDUQSignal.\n", signal);
			break;
	}
}
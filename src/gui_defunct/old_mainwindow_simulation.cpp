/*
	*** Main Window - SimulationTab Functions
	*** src/gui/mainwindow_simulation.cpp
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
#include "base/sysfunc.h"

/*
 * Simulation Control Group
 */

/*!
 * \brief Setup Simulation
 */
void MainWindow::on_SimulationSetupButton_clicked(bool checked)
{
	// Check system setup
	msg.print("Checking system setup...\n");
	if (!MPIRunMaster(dUQ_.checkSetup()))
	{
		msg.print("Errors were encountered while checking the system setup.\nYou must resolve these errors before any calculation can proceed.\n");
		ui.SimulationStartButton->setEnabled(true);
		ui.SimulationStopButton->setEnabled(false);
		return;
	}

	// Broadcast System Setup to slaves
	msg.print("Broadcasting system setup...\n");
	if (!dUQ_.broadcastSetup())
	{
		ui.SimulationStartButton->setEnabled(true);
		ui.SimulationStopButton->setEnabled(false);
		return;
	}

	// Initialise random seed
	if (dUQ_.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dUQ_.seed());

	// Set up Configuration (all processes)
	msg.print("Setting up simulation...\n");
	if (!dUQ_.setupSimulation())
	{
		msg.print("Failed to setup simulation.\n");
		ui.SimulationStartButton->setEnabled(false);
		ui.SimulationStopButton->setEnabled(false);
		return;
	}

	// Set up CheckPoint data structures
	msg.print("Setting up CheckPoint data...\n");
	dUQ_.setupCheckPointData();
	addCheckPointDataToGraphs();
	ui.SimulationViewer->update();
	ui.SimulationSQGraph->refreshData();
	ui.SimulationFQGraph->refreshData();
	
	// Set control enabled status
	ui.SimulationSetupButton->setEnabled(true);
	ui.SimulationStartButton->setEnabled(true);
	ui.SimulationStopButton->setEnabled(false);
}

/*!
 * \brief Start simulation from within GUI
 */
void MainWindow::on_SimulationStartButton_clicked(bool checked)
{
	// Disable all tabs except the Simulation and Messages tabs
	disableTabs(-(SimulationTab+MessagesTab));

	// Disable the Start and LoadCheckPoint buttons, and enable Stop
	ui.SimulationSetupButton->setEnabled(false);
	ui.SimulationStartButton->setEnabled(false);
	ui.SimulationLoadCheckPointButton->setEnabled(false);
	ui.SimulationStopButton->setEnabled(true);

	// Now, start the simulation thread....
	simulationThread_.start();
}

// Stop current simulation running in GUI
void MainWindow::on_SimulationStopButton_clicked(bool checked)
{
	// Temporarily disable GUI
	setEnabled(false);

	// Send terminate signal to dUQ
	dUQ_.receiveSignal(DUQ::TerminateSignal, 898);
}

// Load in checkpoint file
void MainWindow::on_SimulationLoadCheckPointButton_clicked(bool checked)
{
}

/*
 * Signals/Slots
 */

// Simulation(Thread) has finished...
void MainWindow::simulationFinished(int result)
{
	// Re-enable GUI (in case it has been disabled)
	setEnabled(true);

	// Re-enable the Start and LoadCheckPoint buttons, and disable Stop
	ui.SimulationSetupButton->setEnabled(true);
	ui.SimulationStartButton->setEnabled(true);
	ui.SimulationLoadCheckPointButton->setEnabled(true);
	ui.SimulationStopButton->setEnabled(false);
}

// CheckPoint data is ready to read
void MainWindow::addCheckPointDataToGraphs()
{
	// Local Configuration
	ui.SimulationViewer->setSource(&simulationViewerConfiguration_);

	// Simulated S(Q) Data
	ui.SimulationSQGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointUnweightedSQ); ++n)
	{
		QColor color = PlotWidget::lineColour(n);
		ui.SimulationSQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointUnweightedSQ, n), true, n, color);
	}

	// Total F(Q) and reference F(Q)
	ui.SimulationFQGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointFQ); n += 3)
	{
		QColor color = PlotWidget::lineColour(n/3);
		ui.SimulationFQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n), true, (n/3), color);
		ui.SimulationFQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n+1), true, (n/3), color, Qt::DashLine);
		ui.SimulationFQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n+2), true, (n/3), color, Qt::DotLine);
	}
	// -- Set soft limits on graph
	double xMax = 20.0;
	for (Sample* sam = dUQ_.samples(); sam != NULL; sam = sam->next) if (sam->hasReferenceData() && (sam->referenceFQ().arrayX().last() > xMax)) xMax = sam->referenceFQ().arrayX().last();
	ui.SimulationFQGraph->setXLimits(true, true, 0.0, true,	 true, xMax);

	// Unweighted partial g(r)
	ui.SimulationPartialGRGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointUnweightedGR); ++n)
	{
		QColor color = PlotWidget::lineColour(n);
		ui.SimulationPartialGRGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointUnweightedGR, n), true, n, color);
	}
	
	// Total, neutron-weighted g(r)
	ui.SimulationGRGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointTotalGR); ++n)
	{
		QColor color = PlotWidget::lineColour(n);
		ui.SimulationGRGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointTotalGR, n), true, n, color);
	}

	// PairPotentials
	ui.SimulationPPGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointOriginalU); ++n)
	{
		QColor color = PlotWidget::lineColour(n);
		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointU, n), true, 0, color, Qt::SolidLine, PlotData::SqrtModifier);
		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointOriginalU, n), true, 0, color, Qt::DashLine, PlotData::SqrtModifier);
		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointV, n), false, 0, color, Qt::DotLine, PlotData::SqrtModifier);
// 		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointDU, n), false, 0, color, Qt::DashDotDotLine, PlotData::SqrtModifier);
	}
	
	// Energy
	ui.SimulationEnergyGraph->removeAllDataSets();
	ui.SimulationEnergyGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointEnergy, 0), true, 0);
}

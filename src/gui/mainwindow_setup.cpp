/*
	*** Main Window - Setup Functions
	*** src/gui/mainwindow_setup.cpp
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
 * Signals/Slots
 */

// CheckPoint data is ready to read
void MainWindow::addCheckPointDataToGraphs()
{
	// Simulated S(Q) Data
	sqDockWidget_.ui.SQGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointUnweightedSQ); ++n)
	{
		QColor color = PlotWidget::lineColour(n);
		sqDockWidget_.ui.SQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointUnweightedSQ, n), true, n, color);
	}

	// Total F(Q) and reference F(Q)
	fqDockWidget_.ui.FQGraph->removeAllDataSets();
	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointFQ); n += 3)
	{
		QColor color = PlotWidget::lineColour(n/3);
		fqDockWidget_.ui.FQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n), true, (n/3), color);
		fqDockWidget_.ui.FQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n+1), true, (n/3), color, Qt::DashLine);
		fqDockWidget_.ui.FQGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointFQ, n+2), true, (n/3), color, Qt::DotLine);
	}
	// -- Set soft limits on graph
	double xMax = 20.0;
	for (Sample* sam = dUQ_.samples(); sam != NULL; sam = sam->next) if (sam->hasReferenceData() && (sam->referenceFQ().arrayX().last() > xMax)) xMax = sam->referenceFQ().arrayX().last();
	fqDockWidget_.ui.FQGraph->setXLimits(true, true, 0.0, true,	 true, xMax);

	// Unweighted partial g(r)
// 	ui.SimulationPartialGRGraph->removeAllDataSets();
// 	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointUnweightedGR); ++n)
// 	{
// 		QColor color = PlotWidget::lineColour(n);
// 		ui.SimulationPartialGRGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointUnweightedGR, n), true, n, color);
// 	}
// 	
// 	// Total, neutron-weighted g(r)
// 	ui.SimulationGRGraph->removeAllDataSets();
// 	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointTotalGR); ++n)
// 	{
// 		QColor color = PlotWidget::lineColour(n);
// 		ui.SimulationGRGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointTotalGR, n), true, n, color);
// 	}
// 
// 	// PairPotentials
// 	ui.SimulationPPGraph->removeAllDataSets();
// 	for (int n=0; n<dUQ_.nCheckPointData2D(DUQ::CheckPointOriginalU); ++n)
// 	{
// 		QColor color = PlotWidget::lineColour(n);
// 		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointU, n), true, 0, color, Qt::SolidLine, PlotData::SqrtModifier);
// 		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointOriginalU, n), true, 0, color, Qt::DashLine, PlotData::SqrtModifier);
// 		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointV, n), false, 0, color, Qt::DotLine, PlotData::SqrtModifier);
// // 		ui.SimulationPPGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointDU, n), false, 0, color, Qt::DashDotDotLine, PlotData::SqrtModifier);
// 	}
// 	
// 	// Energy
// 	ui.SimulationEnergyGraph->removeAllDataSets();
// 	ui.SimulationEnergyGraph->addDataSet(dUQ_.checkPointData2D(DUQ::CheckPointEnergy, 0), true, 0);
}

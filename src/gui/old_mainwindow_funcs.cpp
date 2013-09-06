/*
	*** Main Window Functions
	*** src/gui/mainwindow_funcs.cpp
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

#include "base/messenger.h"
#include "gui/mainwindow.h"
#include "version.h"

// Constructor
MainWindow::MainWindow(DUQObject& duq) : QMainWindow(parent)
{
	// Call the main creation function
	ui.setupUi(this);
	
	// Store MainWindow pointer in Viewer
	ui.SpeciesViewer->setParentMainWindow(this);
	ui.SpeciesViewer->setAtomContextMenu(ui.menuSelection);
	ui.SpeciesViewer->setSpeciesUpdateTargets(ViewEditGroup);
	
	// Hide Selection menu in GUI
	ui.menuSelection->setVisible(false);

	// Set initial variables
	speciesTabRefreshing_ = false;
	experimentTabRefreshing_ = false;

	// Setup item delegates
	// -- ExperimentComponentsTable
	speciesRelativePopulationDelegate_ = new DoubleSpinDelegate(this, 0.0, 1e6, 1.0, 3);
	ui.SystemCompositionTable->setItemDelegateForColumn(1, speciesRelativePopulationDelegate_);
	// -- ExperimentMixTable
	isotopologueRelativePopulationDelegate_ = new DoubleSpinDelegate(this, 0.0, 1000, 1.0, 3);
	ui.AssignedIsotopologuesTree->setItemDelegateForColumn(2, isotopologueRelativePopulationDelegate_);
	// -- PotentialsTable
	Dnchar items;
	for (int n=0; n<PairPotential::nPairPotentialTypes; ++n) items.strcatf("%s%s", n == 0 ? "" : ",", PairPotential::pairPotentialType( (PairPotential::PairPotentialType) n));
	potentialsStyleDelegate_ = new ComboBoxDelegate(this, items.get());
	ui.PairPotentialsTable->setItemDelegateForColumn(2, potentialsStyleDelegate_);
	potentialsSigmaDelegate_ = new DoubleSpinDelegate(this, 0.0, 10.0, 0.1, 3);
	ui.PairPotentialsTable->setItemDelegateForColumn(3, potentialsSigmaDelegate_);
	potentialsEpsilonDelegate_ = new DoubleSpinDelegate(this, 0.0, 2.0, 0.05, 4);
	ui.PairPotentialsTable->setItemDelegateForColumn(4, potentialsEpsilonDelegate_);
	potentialsChargeDelegate_ = new DoubleSpinDelegate(this, -100.0, 100.0, 0.1, 3);
	ui.PairPotentialsTable->setItemDelegateForColumn(5, potentialsChargeDelegate_);
	ui.PairPotentialsTable->setItemDelegateForColumn(6, potentialsChargeDelegate_);
	// -- SpeciesBondsTable
	bondEquilibriumDelegate_ = new DoubleSpinDelegate(this, 0.5, 1000.0, 0.1, 3);
	ui.SpeciesBondsTable->setItemDelegateForColumn(2, bondEquilibriumDelegate_);
	bondForceConstantDelegate_ = new DoubleSpinDelegate(this, 10.0, 100000.0, 100.0, 1);
	ui.SpeciesBondsTable->setItemDelegateForColumn(3, bondEquilibriumDelegate_);
	// -- SpeciesAnglesTable
	angleEquilibriumDelegate_ = new DoubleSpinDelegate(this, 0.5, 179.0, 1.0, 3);
	ui.SpeciesAnglesTable->setItemDelegateForColumn(3, angleEquilibriumDelegate_);
	angleForceConstantDelegate_ = new DoubleSpinDelegate(this, 10.0, 100000.0, 100.0, 1);
	ui.SpeciesAnglesTable->setItemDelegateForColumn(4, angleEquilibriumDelegate_);

	// Set titles and soft limits (if necessary) on PlotWidgets
	ui.PairPotentialsGraph->setTitles("Pair Potentials", QString("r, ")+QChar(197), "U, kj/mol");
	ui.PairPotentialsGraph->setXLimits(true, true, 0.0, true, false, 200.0);
	ui.PairPotentialsGraph->setYLimits(true, true, -200.0, true, true, 200.0);
	ui.SimulationSQGraph->setTitles("Simulated Partial Structure Factors", QString("Q, 1/")+QChar(197), "S(Q), barns/sr/atom");
	ui.SimulationSQGraph->setXLimits(true, true, 0.0, true,	 true, 20.0);
	ui.SimulationFQGraph->setTitles("Total Structure Factors", QString("Q, 1/")+QChar(197), "F(Q), barns/sr/atom");
	ui.SimulationPartialGRGraph->setTitles("Simulated Partial RDFs", QString("r, ")+QChar(197), "g(r)");
	ui.SimulationPartialGRGraph->setXLimits(true, true, 0.0, true,	 true, 20.0);
	ui.SimulationGRGraph->setTitles("Simulated Total RDFs", QString("r, ")+QChar(197), "g(r)");
	ui.SimulationGRGraph->setXLimits(true, true, 0.0, true,	 true, 20.0);
	ui.SimulationPPGraph->setTitles("Pair Potentials", QString("r, ")+QChar(197), "U, kj/mol");
	ui.SimulationPPGraph->setYLimits(true, true, -100.0, true, true, 100.0);
	ui.SimulationEnergyGraph->setTitles("System Energy", "Step", "U, kj/mol");
	ui.SimulationEnergyGraph->setXLimits(true, true, 0.0, false, true, 0.0);

	// Assign toolbars to PlotWidgets
	ui.PairPotentialsGraph->setToolBarWidget(ui.PairPotentialsGraphToolbar);
	ui.SimulationSQGraph->setToolBarWidget(ui.SimulationSQGraphToolbar);
	ui.SimulationFQGraph->setToolBarWidget(ui.SimulationFQGraphToolbar);
	ui.SimulationPPGraph->setToolBarWidget(ui.SimulationPPGraphToolbar);
	ui.SimulationGRGraph->setToolBarWidget(ui.SimulationGRGraphToolbar);
	ui.SimulationPartialGRGraph->setToolBarWidget(ui.SimulationPartialGRGraphToolbar);
	ui.SimulationEnergyGraph->setToolBarWidget(ui.SimulationEnergyGraphToolbar);

	// Connect controls to Viewer
	ui.SimulationViewer->connect(ui.SimulationViewerStyleCombo, SIGNAL(currentIndexChanged(int)), SLOT(changeDrawStyle(int)));

	// Update GUI where necessary
	setHelpFramesVisible(ui.actionHelpShow->isChecked());

	// Move dUQ execution to its own thread and connect various signals
	dUQ_.moveToThread(&simulationThread_);
	connect(&dUQ_, SIGNAL(simulationFinished(int)), SLOT(simulationFinished(int)));
	dUQ_.connect(&simulationThread_, SIGNAL(started()), SLOT(startSimulation()));
	connect(&dUQ_, SIGNAL(sendDUQSignal(int,int)), SLOT(receiveDUQSignal(int,int)));
	
	// Connect the OutputHandler displayText signal to the QTextEdit, and link it into the main Messenger
	connect(&outputHandler_, SIGNAL(displayText(QString)), ui.MessagesEdit, SLOT(append(QString)), Qt::QueuedConnection);
	msg.setTargetOutputHandler(&outputHandler_);

	CLEAR_MODIFIED
}

// Destructor
MainWindow::~MainWindow()
{
}

// Window close event
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (IS_MODIFIED)
	{
		if (QMessageBox::question(this, "Changes Will be Lost", "Current file is modified and unsaved - really quit?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
		{
			event->ignore();
			return;
		}
	}
}

// General refresh function
void MainWindow::refresh(int targets)
{
	// Update the main window title
	updateTitle();

	// Species Tab
	refreshSpeciesTab(targets&DefinedSpeciesGroup, targets&DefinedAtomTypesGroup, targets&ViewEditGroup, targets&GrainsGroup, targets&IsotopologuesGroup);
	
	// Experiment Tab
	refreshExperimentTab(targets&SystemCompositionGroup, targets&SamplesGroup, targets&AssignedIsotopologuesGroup, targets&ReferenceDataGroup);
	
	// Potential Tab
	refreshPotentialTab(targets&PairPotentialsGroup, targets&PairPotentialsGraph, targets&IntramolecularTermsGroup);
}

// Initialise dUQ
bool MainWindow::initialise(const char* inputFile)
{
	// Load external datafiles
	if (!MPIRunMaster(dUQ_.loadDataFiles()))
	{
		Comm.finalise();
		return 1;
	}
	
	// Broadcast periodic table (including isotope and parameter data)
	if (!dUQ_.periodicTable().broadcast())
	{
		Comm.finalise();
		return 1;
	}

	// Register commands
	msg.print("Registering commands...\n");
	if (!dUQ_.registerCommands())
	{
		Comm.finalise();
		return false;
	}

	// Load existing input file (if specified)
	if (inputFile != NULL)
	{
		if (!dUQ_.loadInput(inputFile)) return false;
		
		CLEAR_MODIFIED
	}
	
	// Update main window title
	updateTitle();
	
	// Redirect output to Messages tab
	outputHandler_.setPrintToConsole(false);
	
	return true;
}

// Return reference to dUQ
GuiDUQ& MainWindow::dUQ()
{
	return dUQ_;
}

/*
 * Tab Control
 */

// Enable specified tabs (use negative for 'all but')
void MainWindow::enableTabs(int tabs)
{
	for (int n=0; n<ui.MainTabs->count(); ++n)
	{
		if (tabs == 0) ui.MainTabs->setTabEnabled(n, true);
		else if ((tabs > 0) && (tabs&(1 << n))) ui.MainTabs->setTabEnabled(n, true);
		else if (!(abs(tabs)&(1 << n))) ui.MainTabs->setTabEnabled(n, true);
	}
}

// Disable specified tabs (use negative for 'all but')
void MainWindow::disableTabs(int tabs)
{
	for (int n=0; n<ui.MainTabs->count(); ++n)
	{
		if (tabs == 0) ui.MainTabs->setTabEnabled(n, false);
		else if ((tabs > 0) && (tabs&(1 << n))) ui.MainTabs->setTabEnabled(n, false);
		else if (!(abs(tabs)&(1 << n))) ui.MainTabs->setTabEnabled(n, false);
	}
}

// Update main window title
void MainWindow::updateTitle()
{
	Dnchar title;
	title.sprintf("dUQ r%s - '%s'", DUQREVISION, dUQ_.fileName());
	if (IS_MODIFIED) title.strcat(" (modified)");
	setWindowTitle(title.get());
}

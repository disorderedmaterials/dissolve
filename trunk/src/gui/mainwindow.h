/*
	*** Main Window
	*** src/gui/mainwindow.h
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

#ifndef DUQ_MAINWINDOW_H
#define DUQ_MAINWINDOW_H

#include "gui/ui_mainwindow.h"
#include "gui/guiduq.uih"
#include "gui/atomtypes.h"
#include "gui/element.h"
#include "gui/samples.h"
#include "gui/species.h"
#include "gui/system.h"
#include "gui/fq.h"
#include "gui/sq.h"

// Forward Declarations
class GrainDefinition;

/** \addtogroup GUI
 *  @{
 */

/*!
 * \brief dUQ Main Window
 * \details TODO
 */
class MainWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	MainWindow(DUQObject& duq);
	~MainWindow();
	// Available Dock Widgets
	enum DockWidgets { SystemWidget=1, SamplesWidget=2, FQWidget=4, SQWidget=8, AllWidgets=65535 };
	// Main form declaration
	Ui::MainWindow ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();
	// Update main window title
	void updateTitle();

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	// General refresh function
	void refresh(int targets);


	/*
	// dUQ Interface
	*/
	private:
	// Reference to main dUQ Object
	DUQObject& dUQ_;
	// GUI OutputHandler
	GuiOutputHandler outputHandler_;
	// Thread to run the simulation in
	QThreadEx simulationThread_;


	/****************************************************************/
	//                           Menus                              //
	/****************************************************************/
	
	/*
	// File Menu
	*/
	private slots:
	void on_actionFileNew_triggered(bool checked);
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);

	/*
	// Windows Menu
	*/
	private slots:
	void on_actionWindowsAtomTypes_triggered(bool checked);
	void on_actionWindowsFQ_triggered(bool checked);
	void on_actionWindowsSamples_triggered(bool checked);
	void on_actionWindowsSpecies_triggered(bool checked);
	void on_actionWindowsSQ_triggered(bool checked);
	void on_actionWindowsSystem_triggered(bool checked);
	void on_actionWindowsCheckSetup_triggered(bool checked);


	/****************************************************************/
	//                           Tabs                               //
	/****************************************************************/
	private:


// 	private:
// 	// Flag to indicate that the PotentialTab is currently being refreshed
// 	bool potentialTabRefreshing_;
// 	// Delegates for PairPotentialsTable
// 	DoubleSpinDelegate* potentialsSigmaDelegate_, *potentialsEpsilonDelegate_, *potentialsChargeDelegate_;
// 	ComboBoxDelegate* potentialsStyleDelegate_;
// 	// Delegates for SpeciesBondsTable
// 	DoubleSpinDelegate* bondEquilibriumDelegate_, *bondForceConstantDelegate_;
// 	// Delegates for SpeciesAnglesTable
// 	DoubleSpinDelegate* angleEquilibriumDelegate_, *angleForceConstantDelegate_;
// 	
// 	private:
// 	// Refresh tab
// 	void refreshPotentialTab(bool pairPotentialsGroup = false, bool pairPotentialsGraph = false, bool intraGroup = false);
// 	// Return selected Potential
// 	PairPotential* selectedPairPotential();
// 	// Update PairPotential Graph to display selected PairPotential
// 	void updatePairPotentialGraph();
// 
// 	// Pair Potentials Group
// 	private slots:
// 	void on_PairPotentialRangeSpin_valueChanged(double value);
// 	void on_PairPotentialTruncationWidthSpin_valueChanged(double value);
// 	void on_PairPotentialDeltaSpin_valueChanged(double value);
// 	void on_PairPotentialsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
// 	void on_PairPotentialsTable_cellChanged(int row, int column);
// 	void on_RemovePairPotentialButton_clicked(bool changed);
// 	void on_AddMissingPairPotentialsButton_clicked(bool changed);
// 
// 	// Intramolecular Group
// 	private slots:
// 	void on_AddBondButton_clicked(bool checked);
// 	void on_RemoveBondButton_clicked(bool checked);
// 	void on_AddAngleButton_clicked(bool checked);
// 	void on_RemoveAngleButton_clicked(bool checked);
// 	void on_RegenerateTermsButton_clicked(bool checked);
	
	/** @} */ // End of GUI_Potential

	/** \defgroup GUI_Simulation Simulation Execution / Check
	 *  \ingroup GUI
	 * The *Simulation* tab defines  TODO
	 * @{
	 */
	private:
	// Flag to indicate that the PotentialTab is currently being refreshed
// 	bool potentialTabRefreshing_;
	// Configuration target for SimulationViewer
	Configuration simulationViewerConfiguration_;

	private:
	// Up

	// Simulation Control Group
	private slots:
// 	void on_SimulationSetupButton_clicked(bool checked);
// 	void on_SimulationStartButton_clicked(bool checked);
// 	void on_SimulationStopButton_clicked(bool checked);
// 	void on_SimulationLoadCheckPointButton_clicked(bool checked);

public:
	// Add CheckPoint data to graphs
	void addCheckPointDataToGraphs();

	public slots:
	// Simulation has finished (for whatever reason)
// 	void simulationFinished(int result);

	/** @} */ // End of GUI_Simulation


	/****************************************************************/
	//                   Dialogs /  Dock Widgets                    //
	/****************************************************************/
	private:
	// Element Select Dialog (Periodic Table)
	DUQElementDialog periodicTable_;
	// AtomTypes Dock Widget
	DUQAtomTypesDockWidget atomTypesDockWidget_;
	// F(Q) Dock Widget
	DUQFQDockWidget fqDockWidget_;
	// Species Dock Widget
	DUQSpeciesDockWidget speciesDockWidget_;
	// Samples Dock Widget
	DUQSamplesDockWidget samplesDockWidget_;
	// S(Q) Dock Widget
	DUQSQDockWidget sqDockWidget_;
	// System Dock Widget
	DUQSystemDockWidget systemDockWidget_;


	/****************************************************************/
	//                        Signals                               //
	/****************************************************************/
	signals:


	/****************************************************************/
	//                         Slots                                //
	/****************************************************************/
	public slots:
	// Signal receiver from GuiDUQ
// 	void receiveDUQSignal(int signal, int data);
};

/** @}*/

#endif

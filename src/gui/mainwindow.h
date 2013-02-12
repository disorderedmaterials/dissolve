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
#include "gui/ptable.h"
#include "gui/integerspindelegate.uih"
#include "gui/doublespindelegate.uih"
#include "gui/comboboxdelegate.uih"
#include "gui/guiduq.uih"

// Forward Declarations
class GrainDefinition;

/** \addtogroup GUI
 *  @{
 */

/*!
 * \brief dUQ Main Window
 * \details The dUQ window consists of a series of tabbed pages, taking you through the setup of a representation of an experimental system,
 * through to the running of the refinement, and on to the analysis of the results.
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
	MainWindow(QMainWindow* parent = 0);
	~MainWindow();
	// Enum of groups in UI
	enum GuiGroups { DefinedSpeciesGroup=1, ViewEditGroup=2, DefinedAtomTypesGroup=4, GrainsGroup=8, IsotopologuesGroup=16, SystemCompositionGroup=32, SamplesGroup=64, AssignedIsotopologuesGroup=128, PairPotentialsGroup=256, PairPotentialsGraph=512, IntramolecularTermsGroup=1024, ReferenceDataGroup=2048, AllGroups=65535};
	// Enum of tabs in UI
	enum GuiTabs { SystemTab=1, ExperimentTab=2, PotentialTab=4, SimulationTab=8, MessagesTab=16 };
	// Main form declaration
	Ui::MainWindow ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();
	// Update main window title
	void updateTitle();

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);
	
	public:
	// General refresh function
	void refresh(int targets = 0);


	/*
	// dUQ Interface
	*/
	private:
	// dUQ main structure
	GuiDUQ dUQ_;
	// GUI OutputHandler
	GuiOutputHandler outputHandler_;
	// Thread to run the simulation in
	QThreadEx simulationThread_;

	public:
	// Initialise dUQ
	bool initialise(const char* inputFile = NULL);
	// Return reference to dUQ
	GuiDUQ& dUQ();


	/*
	 * Tab Control
	 */
	private:
	// Enable specified tabs (use negative for 'all but')
	void enableTabs(int tabs);
	// Disable specified tabs (use negative for 'all but')
	void disableTabs(int tabs);


	/****************************************************************/
	//                           Menus                              //
	/****************************************************************/
	
	/*
	// File Menu
	*/
	private slots:
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);
	
	/*
	// Help Menu
	*/
	private:
	// Show/Hide help frames
	void setHelpFramesVisible(bool visible);
	
	/*
	// Tools Menu
	*/
	private slots:
// 	void on_actionToolsCheckSetup_triggered(bool checked);

	private slots:
	void on_actionHelpShow_triggered(bool checked);
	
	/*
	// Selection Menu
	*/
	private:
	// Transmute selection
	void transmuteSelection(int el);

	private slots:
	void on_actionSelectionHydrogen_triggered(bool checked);
	void on_actionSelectionCarbon_triggered(bool checked);
	void on_actionSelectionNitrogen_triggered(bool checked);
	void on_actionSelectionOxygen_triggered(bool checked);
	void on_actionSelectionSilicon_triggered(bool checked);
	void on_actionSelectionPhosphorous_triggered(bool checked);
	void on_actionSelectionSulfur_triggered(bool checked);
	void on_actionSelectionOther_triggered(bool checked);
	void on_actionSelectionSelectSame_triggered(bool checked);

	public slots:
	void selectionAtomTypeChanged(bool checked);


	/****************************************************************/
	//                           Tabs                               //
	/****************************************************************/

	/** \defgroup GUI_Species Species Setup
	 *  \ingroup GUI
	 * The *Species Setup* tab is the starting point for any setup, and allows you to define atomic representations of 
	 * one or more Species which exist in the experimental system.
	 * @{
	 */
	private:
	// Flag to indicate that the SpeciesTab is currently being refreshed
	bool speciesTabRefreshing_;

	private:
	// Refresh tab
	void refreshSpeciesTab(bool listGroup = TRUE, bool atomTypesGroup = TRUE, bool viewGroup = TRUE, bool grainsGroup = TRUE, bool isotopologuesGroup = TRUE);
	// Return current selected Species (if any)
	Species* selectedSpecies();
	// Return current selected GrainDefinition, if any
	GrainDefinition* selectedGrainDefinition();
	// Return current selected Isotopologue, if any
	Isotopologue* selectedIsotopologue();
	// Return current selected AtomType, if any
	AtomType* selectedAtomType();

	// Defined Species Group
	private slots:
	void on_DefinedSpeciesList_currentRowChanged(int row);
	void on_DefinedSpeciesList_itemChanged(QListWidgetItem* item);
	void on_AddSpeciesButton_clicked(bool checked);
	void on_RemoveSpeciesButton_clicked(bool checked);
	void on_LoadSpeciesButton_clicked(bool checked);
	void on_SaveSpeciesButton_clicked(bool checked);

	// Species Viewer
	private slots:
	void on_AddBondFromSelectionButton_clicked(bool checked);
	void on_AddAngleFromSelectionButton_clicked(bool checked);
	void on_AddGrainFromSelectionButton_clicked(bool checked);
	void on_ViewAtomTypeCheck_clicked(bool checked);
	void on_ViewIndexCheck_clicked(bool checked);

	// Defined Atom Types Group
	public slots:
	void atomTypeParametersChanged(int index);
	private slots:
	void on_DefinedAtomTypesTree_itemClicked(QTreeWidgetItem* item, int column);
	void on_DefinedAtomTypesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddAtomTypeButton_clicked(bool checked);
	void on_CopyAtomTypeButton_clicked(bool checked);
	void on_RemoveAtomTypeButton_clicked(bool checked);

	// Grains Group
	private slots:
	void on_GrainsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_GrainsTable_cellChanged(int row, int column);
	void on_AddGrainButton_clicked(bool checked);
	void on_RemoveGrainButton_clicked(bool checked);
	void on_AddDefaultGrainButton_clicked(bool checked);
	void on_AutoGrainButton_clicked(bool checked);
	
	// Isotopologues Group
	private:
	bool getIsotopologueAndAtomType(QStringList data, Isotopologue*& iso, AtomType*& at);
	private slots:
	void on_IsotopologuesTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_IsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddIsotopologueToSpeciesButton_clicked(bool checked);
	void on_RemoveIsotopologueFromSpeciesButton_clicked(bool checked);
	void on_CopyIsotopologueButton_clicked(bool checked);
	public slots:
	void isotopologueIsotopeChanged(int index);

	/** @} */ // End of GUI_Species
	
	/** \defgroup GUI_Experiment Experiment Setup
	 *  \ingroup GUI
	 * The *Experiment Setup* tab defines the contents of the actual experimental system which was measured, in terms
	 * of the relative (that is, molar) populations of each.
	 * TODO Samples
	 * @{
	 */
	private:
	// Delegates for ExperimentComponentsTable
	DoubleSpinDelegate* speciesRelativePopulationDelegate_;
	// Delegates for ExperimentMixTable
	DoubleSpinDelegate* isotopologueRelativePopulationDelegate_;
	// Flag to indicate that the ExperimentTab is currently being refreshed
	bool experimentTabRefreshing_;

	private:
	// Refresh the ExperimentTab
	void refreshExperimentTab(bool compositionGroup = TRUE, bool sampleGroup = TRUE, bool mixGroup = TRUE, bool refDataGroup = TRUE);
	// Return pointer to selected Sample
	Sample* selectedSample();

	// System Composition Group
	private slots:
	void on_SystemCompositionTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_SystemCompositionTable_cellChanged(int row, int column);
	void on_SystemMultiplierSpin_valueChanged(int value);
	void on_SystemDensitySpin_valueChanged(double value);
	void on_SystemDensityUnitsCombo_currentIndexChanged(int index);
	void on_SystemBoxRelativeASpin_valueChanged(double value);
	void on_SystemBoxRelativeBSpin_valueChanged(double value);
	void on_SystemBoxRelativeCSpin_valueChanged(double value);
	void on_SystemBoxAlphaSpin_valueChanged(double value);
	void on_SystemBoxBetaSpin_valueChanged(double value);
	void on_SystemBoxGammaSpin_valueChanged(double value);

	// Samples Group
	private slots:
	void on_SamplesList_currentRowChanged(int row);
	void on_SamplesList_itemChanged(QListWidgetItem* item);
	void on_AddSampleButton_clicked(bool checked);
	void on_RemoveSampleButton_clicked(bool checked);
	void on_CopySampleButton_clicked(bool checked);

	// Assigned Isotopologues Group
	private:
	bool getSpeciesAndIsotopologue(QStringList data, Species*& species, Isotopologue*& iso);
	private slots:
	void on_AssignedIsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddIsotopologueToMixButton_clicked(bool checked);
	void on_RemoveIsotopologueFromMixButton_clicked(bool checked);

	// Reference Data Group
	private slots:
	void on_ReferenceDataFileSelectButton_clicked(bool checked);
	void on_ReferenceDataFilenameEdit_textEdited(const QString& text);
	void on_ReferenceDataNormalisationComboCheck_currentIndexChanged(int index);
	void on_ReferenceDataSubtractSelfCheck_clicked(bool checked);

	public slots:
	void mixtureIsotopologueChanged(int index);

	/** @} */ // End of GUI_Experiment

	/** \defgroup GUI_Potential Potentials Setup
	 *  \ingroup GUI
	 * The *Potential Setup* tab defines  TODO
	 * @{
	 */
	private:
	// Flag to indicate that the PotentialTab is currently being refreshed
	bool potentialTabRefreshing_;
	// Delegates for PairPotentialsTable
	DoubleSpinDelegate* potentialsSigmaDelegate_, *potentialsEpsilonDelegate_, *potentialsChargeDelegate_;
	ComboBoxDelegate* potentialsStyleDelegate_;
	// Delegates for SpeciesBondsTable
	DoubleSpinDelegate* bondEquilibriumDelegate_, *bondForceConstantDelegate_;
	// Delegates for SpeciesAnglesTable
	DoubleSpinDelegate* angleEquilibriumDelegate_, *angleForceConstantDelegate_;
	
	private:
	// Refresh tab
	void refreshPotentialTab(bool pairPotentialsGroup = FALSE, bool pairPotentialsGraph = FALSE, bool intraGroup = FALSE);
	// Return selected Potential
	PairPotential* selectedPairPotential();
	// Update PairPotential Graph to display selected PairPotential
	void updatePairPotentialGraph();

	// Pair Potentials Group
	private slots:
	void on_PairPotentialRangeSpin_valueChanged(double value);
	void on_PairPotentialTruncationWidthSpin_valueChanged(double value);
	void on_PairPotentialDeltaSpin_valueChanged(double value);
	void on_PairPotentialsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_PairPotentialsTable_cellChanged(int row, int column);
	void on_RemovePairPotentialButton_clicked(bool changed);
	void on_AddMissingPairPotentialsButton_clicked(bool changed);

	// Intramolecular Group
	private slots:
	void on_AddBondButton_clicked(bool checked);
	void on_RemoveBondButton_clicked(bool checked);
	void on_AddAngleButton_clicked(bool checked);
	void on_RemoveAngleButton_clicked(bool checked);
	void on_RegenerateTermsButton_clicked(bool checked);
	
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
	void on_SimulationSetupButton_clicked(bool checked);
	void on_SimulationStartButton_clicked(bool checked);
	void on_SimulationStopButton_clicked(bool checked);
	void on_SimulationLoadCheckPointButton_clicked(bool checked);

	private:
	// Add CheckPoint data to graphs
	void addCheckPointDataToGraphs();

	public slots:
	// Simulation has finished (for whatever reason)
	void simulationFinished(int result);

	/** @} */ // End of GUI_Simulation


	/****************************************************************/
	//                        Dialogs                               //
	/****************************************************************/
	private:
	// Periodic Table (element selector)
	dUQPeriodicTable* periodicTable_;


	/****************************************************************/
	//                        Signals                               //
	/****************************************************************/
	signals:


	/****************************************************************/
	//                         Slots                                //
	/****************************************************************/
	public slots:
	// Signal receiver from GuiDUQ
	void receiveDUQSignal(int signal, int data);
};

/** @}*/

#endif

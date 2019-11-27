/*
	*** Dissolve Main Window
	*** src/gui/gui.h
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

#ifndef DISSOLVE_MAINWINDOW_H
#define DISSOLVE_MAINWINDOW_H

#include "gui/ui_gui.h"
#include "gui/outputhandler.hui"
#include "gui/systemtemplate.h"
#include "gui/thread.hui"
#include "gui/maintab.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
class BrowserWidget;
class Configuration;
class ConfigurationTab;
class Dissolve;
class ForcefieldTab;
class QLCDNumber;
class QMdiSubWindow;
class Species;
class SpeciesTab;
class ModuleTab;
class ModuleLayer;
class LayerTab;
class WorkspaceTab;

class DissolveWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	DissolveWindow(Dissolve& dissolve);
	~DissolveWindow();


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::DissolveWindow ui_;

	protected:
	void closeEvent(QCloseEvent* event);
	void resizeEvent(QResizeEvent* event);


	/*
	 * Dissolve Integration
	 */
	private:
	// Dissolve reference
	Dissolve& dissolve_;
	// Whether any data has been modified
	bool modified_;
	// Whether window is currently refreshing
	bool refreshing_;
	// Whether window has been shown
	bool shown_;
	// Output handler for messaging in GUI
	GUIOutputHandler outputHandler_;
	// Whether the current simulation is on the local machine
	bool localSimulation_;

	public slots:
	// Flag that data has been modified via the GUI
	void setModified();

	public:
	// Return reference to Dissolve
	Dissolve& dissolve();
	// Return const reference to Dissolve
	const Dissolve& constDissolve() const;
	// Link the Messenger in to the GUI output device
	void addOutputHandler();


	/*
	 * StatusBar
	 */
	private:
	// Label for local / remote simulation indicator
	QLabel* localSimulationIndicator_;
	// Label for restart file indicator
	QLabel* restartFileIndicator_;
	// Label for heartbeat file indicator
	QLabel* heartbeatFileIndicator_;


	/*
	 * File
	 */
	public:
	// Open specified input file
	bool openLocalFile(const char* inputFile, const char* restartFile, bool ignoreRestartFile, bool ignoreLayoutFile);


	/*
	 * System Templates
	 */
	private:
	// List of available SystemTemplates
	List<SystemTemplate> systemTemplates_;

	private:
	// Initialise system templates from the main resource
	void initialiseSystemTemplates();


	/*
	 * Update Functions
	 */
	public slots:
	// Update all tabs
	void updateTabs();
	// Update window title
	void updateWindowTitle();
	// Update controls frame
	void updateControlsFrame();
	// Update menus
	void updateMenus();
	// Perform full update of the GUI, including tab reconciliation
	void fullUpdate();
	// Update all Species tabs
	void updateSpeciesTabs();


	/*
	 * Main Menu
	 */
	private:
	// Check whether current input needs to be saved and, if so, if it saved successfully
	bool checkSaveCurrentInput();
	// Clear all data and start new simulation afresh
	void startNew();

	private slots:
	// File
	void on_FileNewAction_triggered(bool checked);
	void on_FileNewFromTemplateAction_triggered(bool checked);
	void on_FileOpenLocalAction_triggered(bool checked);
	void on_FileOpenRecentAction_triggered(bool checked);
	void on_FileConnectAction_triggered(bool checked);
	void on_FileCloseAction_triggered(bool checked);
	void on_FileSaveAction_triggered(bool checked);
	void on_FileSaveAsAction_triggered(bool checked);
	void on_FileQuitAction_triggered(bool checked);
	// Simulation
	void on_SimulationRunAction_triggered(bool checked);
	void on_SimulationStepAction_triggered(bool checked);
	void on_SimulationStepFiveAction_triggered(bool checked);
	void on_SimulationPauseAction_triggered(bool checked);
	void on_SimulationSetRandomSeedAction_triggered(bool checked);
	// Species
	void on_SpeciesCreateEmptyAction_triggered(bool checked);
	void on_SpeciesCreateAtomicAction_triggered(bool checked);
	void on_SpeciesImportFromDissolveAction_triggered(bool checked);
	void on_SpeciesImportFromXYZAction_triggered(bool checked);
	void on_SpeciesRenameAction_triggered(bool checked);
	void on_SpeciesAddForcefieldTermsAction_triggered(bool checked);
	void on_SpeciesDeleteAction_triggered(bool checked);
	// Configuration
	void on_ConfigurationCreateEmptyAction_triggered(bool checked);
	void on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked);
	void on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked);
	void on_ConfigurationRenameAction_triggered(bool checked);
	void on_ConfigurationDeleteAction_triggered(bool checked);
	void on_ConfigurationExportToXYZAction_triggered(bool checked);
	// Layer
	void on_LayerCreateEmptyAction_triggered(bool checked);
	void on_LayerCreateEvolutionMolecularAction_triggered(bool checked);
	void on_LayerCreateEvolutionAtomicAction_triggered(bool checked);
	void on_LayerCreateEvolutionEPSRAction_triggered(bool checked);
	void on_LayerCreateRefinementEPSRAction_triggered(bool checked);
	void on_LayerCreateCalculateRDFAction_triggered(bool checked);
	void on_LayerCreateCalculateRDFStructureFactorAction_triggered(bool checked);
	void on_LayerCreateCalculateRDFNeutronAction_triggered(bool checked);
	void on_LayerCreateCalculateRDFNeutronXRayAction_triggered(bool checked);
	void on_LayerRenameAction_triggered(bool checked);
	void on_LayerDeleteAction_triggered(bool checked);
	// Workspace
	void on_WorkspaceCreateEmptyAction_triggered(bool checked);
	// Help
	void on_HelpOnlineManualAction_triggered(bool checked);
	void on_HelpOnlineTutorialsAction_triggered(bool checked);


	/*
	 * Main Stack
	 */
	private:
	// Stack Pages Enum
	enum MainStackPage
	{
		StartStackPage,		/* Start Page - Routes to load, create, and monitor simulations */
		SimulationStackPage,	/* Simulation Page - Controls for current simulation */
		nStackPages
	};

	private:
	// Set currently-visible main stack page
	void showMainStackPage(MainStackPage page);


	/*
	 * 'Start' Stack Page
	 */
	private slots:
	// 'Create' Group
	void on_StartCreateNewButton_clicked(bool checked);
	void on_StartCreateFromTemplateButton_clicked(bool checked);
	// 'Open / Connect' Group
	void on_StartOpenLocalButton_clicked(bool checked);
	void on_StartOpenRecentButton_clicked(bool checked);
	void on_StartConnectButton_clicked(bool checked);
	// Help
	void on_StartOnlineManualButton_clicked(bool checked);
	void on_StartOnlineTutorialsButton_clicked(bool checked);


	/*
	 * 'Simulation' Stack Page - Run Control
	 */
	public:
	// Dissolve State Enum
	enum DissolveState {
		EditingState,		/* Dissolve is currently editing a file in the GUI */
		RunningState,		/* Dissolve is currently running in the GUI */
		MonitoringState,	/* Dissolve is running in the background, and we are monitoring it via the restart file */
		nDissolveStates
	};

	private:
	// Thread controller
	DissolveThreadController threadController_;
	// Current state of Dissolve
	DissolveState dissolveState_;

	public:
	// Return current state of Dissolve
	DissolveState dissolveState() const;

	private slots: 
	void on_ControlRunButton_clicked(bool checked);
	void on_ControlStepButton_clicked(bool checked);
	void on_ControlPauseButton_clicked(bool checked);
	void on_ControlReloadButton_clicked(bool checked);

	public slots:
	// Set widgets ready for the main code to be run
	void setWidgetsForRun();
	// Set widgets after the main code has been run
	void setWidgetsAfterRun();
	// All iterations requested are complete
	void iterationsComplete();

	signals:
	void iterate(int);
	void stopIterating();


	/*
	 * 'Simulation' Stack Page - Tabs
	 */
	private:
	// Pointer to Forcefield tab
	ForcefieldTab* forcefieldTab_;
	// List of Species tabs
	List<SpeciesTab> speciesTabs_;
	// List of Configuration tabs
	List<ConfigurationTab> configurationTabs_;
	// List of processing layer tabs
	List<LayerTab> processingLayerTabs_;
	// List of Module tabs
	List<ModuleTab> moduleTabs_;
	// List of Workspace tabs
	List<WorkspaceTab> workspaceTabs_;

	private slots:
	void on_MainTabs_currentChanged(int index);

	private:
	// Remove tabs related to the current data
	void clearTabs();
	// Reconcile tabs, making them consistent with the current data
	void reconcileTabs();
	// Add core (permanent) tabs
	void addCoreTabs();
	// Add on an empty workspace tab
	MainTab* addWorkspaceTab(const char* title);
	// Generate unique Workspace name with base name provided
	const char* uniqueWorkspaceName(const char* base);
	// Find tab with title specified
	MainTab* findTab(const char* title);
	// Find tab with specified page widget
	MainTab* findTab(QWidget* page);
	// Find SpeciesTab containing specified page widget
	SpeciesTab* speciesTab(QWidget* page);
	// Find ConfigurationTab containing specified page widget
	ConfigurationTab* configurationTab(QWidget* page);
	// Find LayerTab containing specified page widget
	LayerTab* processingLayerTab(QWidget* page);
	// Find ModuleTab containing specified page widget
	ModuleTab* moduleTab(QWidget* page);
	// Find ModuleTab containing specified Module
	ModuleTab* moduleTab(Module* module);
	// Find WorkspaceTab containing specified page widget
	WorkspaceTab* workspaceTab(QWidget* page);
	// Return current tab
	MainTab* currentTab() const;
	// Make specified tab the current one
	void setCurrentTab(MainTab* tab);
	// Make specified tab the current one (by index)
	void setCurrentTab(int tabIndex);
	// Show forcefield tab
	void showForcefieldTab();
	// Make specified Species tab the current one
	void setCurrentTab(Species* species);
	// Make specified Configuration tab the current one
	void setCurrentTab(Configuration* cfg);
	// Make specified processing layer tab the current one
	void setCurrentTab(ModuleLayer* layer);
	// Return currently-selected Species (if a SpeciesTab is the current one)
	Species* currentSpecies() const;
	// Return currently-selected Configuration (if a ConfigurationTab is the current one)
	Configuration* currentConfiguration() const;
	// Return currently-selected ModuleLayer (if a LayerTab is the current one)
	ModuleLayer* currentLayer() const;

	public:
	// Add tab for specified Module target
	MainTab* addModuleTab(Module* module);
	// Remove the ModuleTab for the specifeid Module, if it exists
	void removeModuleTab(Module* module);
	// Return reference list of all current tabs
	RefList<MainTab> allTabs() const;

	public slots:
	// Add or go to Module tab for the Module with the unique name provided
	void showModuleTab(const QString& uniqueName);
	// Remove the Module tab (if it exists) for the Module with the unique name provided
	void removeModuleTab(const QString& uniqueName);
	// Remove tab containing the specified page widget
	void removeTab(QWidget* page);


	/*
	 * 'Simulation' Stack Page - State I/O
	 */
	private:
	// Filename containing current window layout
	CharString windowLayoutFilename_;

	public:
	// Save current window layout
	bool saveWindowLayout();
	// Load window state
	bool loadWindowLayout();
};

#endif

/*
    *** Dissolve Main Window
    *** src/gui/gui.h
    Copyright T. Youngs 2012-2020

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

#pragma once

#include "gui/maintab.h"
#include "gui/outputhandler.hui"
#include "gui/referencepoint.h"
#include "gui/systemtemplate.h"
#include "gui/thread.hui"
#include "gui/ui_gui.h"
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
    DissolveWindow(Dissolve &dissolve);
    ~DissolveWindow();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DissolveWindow ui_;

    protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

    /*
     * Dissolve Integration
     */
    private:
    // Dissolve reference
    Dissolve &dissolve_;
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
    Dissolve &dissolve();
    // Return const reference to Dissolve
    const Dissolve &constDissolve() const;
    // Link the Messenger in to the GUI output device
    void addOutputHandler();

    /*
     * StatusBar
     */
    private:
    // Label for local / remote simulation indicator
    QLabel *localSimulationIndicator_;
    // Label for restart file indicator
    QLabel *restartFileIndicator_;
    // Label for heartbeat file indicator
    QLabel *heartbeatFileIndicator_;
    // Label for simulation ETA (when using RunFor)
    QLabel *etaLabel_;

    /*
     * File
     */
    public:
    // Open specified input file
    bool openLocalFile(std::string_view inputFile, std::string_view restartFile, bool ignoreRestartFile, bool ignoreLayoutFile);

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
     * Reference Points
     */
    private:
    // List of ReferencePoints currently loaded
    List<ReferencePoint> referencePoints_;

    /*
     * Update Functions
     */
    public slots:
    // Update window title
    void updateWindowTitle();
    // Update controls frame
    void updateControlsFrame();
    // Update menus
    void updateMenus();
    // Perform full update of the GUI, including tab reconciliation
    void fullUpdate();
    // Update while running
    void updateWhileRunning(int iterationsRemaining);

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
    void on_SimulationRunForAction_triggered(bool checked);
    void on_SimulationStepAction_triggered(bool checked);
    void on_SimulationStepFiveAction_triggered(bool checked);
    void on_SimulationPauseAction_triggered(bool checked);
    void on_SimulationSaveRestartPointAction_triggered(bool checked);
    void on_SimulationDataManagerAction_triggered(bool checked);
    void on_SimulationClearModuleDataAction_triggered(bool checked);
    void on_SimulationSetRandomSeedAction_triggered(bool checked);
    // Species
    void on_SpeciesCreateAtomicAction_triggered(bool checked);
    void on_SpeciesCreateDrawAction_triggered(bool checked);
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
    void on_LayerCreateEvolveBasicAtomicAction_triggered(bool checked);
    void on_LayerCreateEvolveAtomicAction_triggered(bool checked);
    void on_LayerCreateEvolveMolecularAction_triggered(bool checked);
    void on_LayerCreateEvolveEPSRAction_triggered(bool checked);
    void on_LayerCreateRefineEPSRAction_triggered(bool checked);
    void on_LayerCreateCalculateRDFAction_triggered(bool checked);
    void on_LayerCreateCalculateRDFStructureFactorAction_triggered(bool checked);
    void on_LayerCreateCalculateRDFNeutronAction_triggered(bool checked);
    void on_LayerCreateCalculateRDFNeutronXRayAction_triggered(bool checked);
    void on_LayerCreateAnalyseRDFCNAction_triggered(bool checked);
    void on_LayerCreateAnalyseAvgMolSDFAction_triggered(bool checked);
    void on_LayerRenameAction_triggered(bool checked);
    void on_LayerDeleteAction_triggered(bool checked);
    // Workspace
    void on_WorkspaceCreateEmptyAction_triggered(bool checked);
    void on_WorkspaceRenameCurrentGizmoAction_triggered(bool checked);
    // Help
    void on_HelpOnlineManualAction_triggered(bool checked);
    void on_HelpOnlineTutorialsAction_triggered(bool checked);

    public slots:
    void currentWorkspaceGizmoChanged(QMdiSubWindow *gizmoWindow);

    /*
     * Main Stack
     */
    private:
    // Stack Pages Enum
    enum MainStackPage
    {
        StartStackPage,      /* Start Page - Routes to load, create, and monitor simulations */
        SimulationStackPage, /* Simulation Page - Controls for current simulation */
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
    enum DissolveState
    {
        EditingState,    /* Dissolve is currently editing a file in the GUI */
        RunningState,    /* Dissolve is currently running in the GUI */
        MonitoringState, /* Dissolve is running in the background, and we are monitoring it via the restart file */
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
    // Disable sensitive controls
    void disableSensitiveControls();
    // Enable sensitive controls
    void enableSensitiveControls();
    // All iterations requested are complete
    void iterationsComplete();

    signals:
    void iterate(int);
    void stopIterating();

    /*
     * 'Simulation' Stack Page - Tabs
     */
    private slots:
    void on_MainTabs_currentChanged(int index);

    public:
    // Return list of all current tabs
    RefList<const MainTab> allTabs() const;

    public slots:
    // Add or go to Module tab for the Module with the unique name provided
    void showModuleTab(const QString &uniqueName);
    // Remove the Module tab (if it exists) for the Module with the unique name provided
    void removeModuleTab(const QString &uniqueName);

    /*
     * 'Simulation' Stack Page - Messages
     */
    private slots:
    void on_MessagesIncreaseFontSizeButton_clicked(bool checked);
    void on_MessagesDecreaseFontSizeButton_clicked(bool checked);
    void clearMessages();
    void appendMessage(const QString &msg);

    /*
     * GUI State
     */
    private:
    // Filename containing current GUI state
    QString stateFilename_;

    public:
    // Save current GUI state
    bool saveState();
    // Load GUI state
    bool loadState();
};

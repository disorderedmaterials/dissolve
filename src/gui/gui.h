// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/referencepoint.h"
#include "gui/maintab.h"
#include "gui/outputhandler.hui"
#include "gui/thread.hui"
#include "gui/ui_gui.h"

// Forward Declarations
class Configuration;
class ConfigurationTab;
class Dissolve;
class ForcefieldTab;
class QMdiSubWindow;
class Species;
class SpeciesTab;
class ModuleLayer;
class LayerTab;
class WorkspaceTab;

class DissolveWindow : public QMainWindow
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DissolveWindow(Dissolve &dissolve);
    ~DissolveWindow() = default;

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

    private:
    // Prepare the simulation and run for a set count
    void setupIteration(int count);

    public slots:
    // Flag that data has been modified via the GUI
    void setModified();

    public:
    // Return reference to Dissolve
    Dissolve &dissolve();
    const Dissolve &dissolve() const;

    /*
     * StatusBar
     */
    private:
    // Label for restart file indicator
    QLabel *restartFileIndicator_;
    // Label for iteration number
    QLabel *iterationLabel_;
    // Label for simulation ETA (when using RunFor)
    QLabel *etaLabel_;
    // General status indicator and label
    QLabel *statusIndicator_, *statusLabel_;

    private:
    // Add text label to status bar
    QLabel *addStatusBarLabel(QString text, bool permanent = true);
    // Add text label to status bar
    QLabel *addStatusBarIcon(QString resource, bool permanent = true);

    /*
     * File
     */
    public:
    // Open specified input file
    bool openLocalFile(std::string_view inputFile, std::optional<std::string_view> restartFile = std::nullopt,
                       bool ignoreRestartFile = false);

    /*
     * Reference Points
     */
    private:
    // List of ReferencePoints currently loaded
    std::vector<ReferencePoint> referencePoints_;

    /*
     * Recent Files
     */
    private:
    // Convenience list of recent file menu actions
    QList<QAction *> recentFileActionList_;
    // Maximum number of recent files to retain
    const int recentFileLimit_;

    public slots:
    // Manage opening of recent files
    void openRecent();
    // Fill open recent with placeholders
    void createRecentMenu();
    // Add current file to recent files
    void addRecentFile(const QString &filePath);
    // Update Recent files menu
    void updateRecentActionList();

    /*
     * Update Functions
     */
    public slots:
    // Update window title
    void updateWindowTitle();
    // Update status bar
    void updateStatusBar();
    // Update menus
    void updateMenus();
    // Perform full update of the GUI, including tab reconciliation
    void fullUpdate();
    // Update while running
    void updateWhileRunning(int iterationsRemaining);
    // Clear the messages window
    void clearMessages();

    /*
     * Main Menu
     */
    private:
    // Check whether current input needs to be saved and, if so, if it saved successfully
    bool checkSaveCurrentInput();
    // Clear all data and start new simulation afresh
    void startNew();

    public slots:
    // File
    void on_FileNewAction_triggered(bool checked);
    void on_FileOpenAction_triggered(bool checked);
    void on_FileCloseAction_triggered(bool checked);
    void on_FileSaveAction_triggered(bool checked);
    void on_FileSaveAsAction_triggered(bool checked);
    void on_FileQuitAction_triggered(bool checked);
    // Simulation
    void on_SimulationCheckAction_triggered(bool checked);
    void on_SimulationRunAction_triggered(bool checked);
    void on_SimulationRunForAction_triggered(bool checked);
    void on_SimulationStepAction_triggered(bool checked);
    void on_SimulationStepFiveAction_triggered(bool checked);
    void on_SimulationStopAction_triggered(bool checked);
    void on_SimulationSaveRestartPointAction_triggered(bool checked);
    void on_SimulationDataManagerAction_triggered(bool checked);
    void on_SimulationClearModuleDataAction_triggered(bool checked);
    void on_SimulationSetRandomSeedAction_triggered(bool checked);
    // Species
    void on_SpeciesCreateAtomicAction_triggered(bool checked);
    void on_SpeciesCreateDrawAction_triggered(bool checked);
    void on_SpeciesImportFromDissolveAction_triggered(bool checked);
    void on_SpeciesImportFromXYZAction_triggered(bool checked);
    void on_SpeciesImportLigParGenAction_triggered(bool checked);
    void on_SpeciesImportFromCIFAction_triggered(bool checked);
    void on_SpeciesRenameAction_triggered(bool checked);
    void on_SpeciesDeleteAction_triggered(bool checked);
    void on_SpeciesAddForcefieldTermsAction_triggered(bool checked);
    void on_SpeciesSimplifyAtomTypesAction_triggered(bool checked);
    void on_SpeciesReduceToMasterTermsAction_triggered(bool checked);
    void on_SpeciesCopyTermsAction_triggered(bool checked);
    void on_SpeciesRegenerateIntraFromConnectivityAction_triggered(bool checked);
    void on_SpeciesSetAtomTypesInSelectionAction_triggered(bool checked);
    void on_SpeciesSetChargesInSelectionAction_triggered(bool checked);
    // Configuration
    void on_ConfigurationCreateEmptyAction_triggered(bool checked);
    void on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked);
    void on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked);
    void on_ConfigurationCreateEmptyFrameworkAction_triggered(bool checked);
    void on_ConfigurationCreateFrameworkAdsorbatesAction_triggered(bool checked);
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
     * Simulation
     */
    private:
    // Thread controller
    DissolveThreadController threadController_;
    // Whether the GUI is currently running a simulation
    bool dissolveIterating_;
    // Whether any data has been modified
    bool modified_;
    // Whether window is currently refreshing
    bool refreshing_;

    private slots:
    void on_MainTabs_currentChanged(int index);

    public:
    // Return whether the GUI is currently running a simulation
    bool dissolveIterating() const;
    // Return list of all current tabs
    const std::vector<MainTab *> allTabs() const;

    public slots:
    // Disable sensitive controls
    void disableSensitiveControls();
    // Enable sensitive controls
    void enableSensitiveControls();
    // All iterations requested are complete
    void iterationsComplete();
    // Specified tab (indicated by page widget) has been closed, and relevant data should be deleted accordingly
    void closeTab(QWidget *page);

    signals:
    void iterate(int);
    void stopIterating();
};

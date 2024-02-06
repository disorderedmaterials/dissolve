// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/timer.h"
#include "gui/mainTab.h"
#include "gui/outputHandler.hui"
#include "gui/signals.h"
#include "gui/thread.hui"
#include "gui/ui_gui.h"

// Forward Declarations
class Configuration;
class ConfigurationTab;
class Dissolve;
class ForcefieldTab;
class Species;
class SpeciesTab;
class ModuleLayer;
class LayerTab;
class ModifyChargesModel;

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

    public:
    // Return reference to Dissolve
    Dissolve &dissolve();
    const Dissolve &dissolve() const;

    /*
     * Data Mutation Signalling
     */
    public Q_SLOTS:
    // Flag that data has been modified via the GUI
    void setModified(Flags<DissolveSignals::DataMutations> dataMutationFlags = {});

    Q_SIGNALS:
    void dataMutated(int);

    /*
     * StatusBar
     */
    private:
    // Label for restart file indicator
    QLabel *restartFileIndicator_;
    // Label for iteration number
    QLabel *iterationLabel_;
    // Label for simulation timer (ETA if RunFor, time elapsed if Run)
    QLabel *timerLabel_;
    // General status indicator and label
    QLabel *statusIndicator_, *statusLabel_;
    // Main loop iteration timer
    Timer elapsedTimer_{Timer(false)};

    private:
    // Add text label to status bar
    QLabel *addStatusBarLabel(QString text, bool permanent = true);
    // Add text label to status bar
    QLabel *addStatusBarIcon(QString resource, bool permanent = true);

    /*
     * File
     */
    public:
    // Load specified input file, and optionally handle restart file choice as well
    bool loadInputFile(std::string_view inputFile, bool handleRestartFile = false);
    // Load specified restart file
    bool loadRestartFile(std::string_view restartFile);

    /*
     * Update Functions
     */
    public Q_SLOTS:
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
    // Convenience list of recent file menu actions
    QList<QAction *> recentFileActionList_;
    // Maximum number of recent files to retain
    const int recentFileLimit_;

    private:
    // Check whether current input needs to be saved and, if so, if it saved successfully
    bool checkSaveCurrentInput();
    // Clear all data and start new simulation afresh
    void startNew();
    // Initialise recent file menu with placeholder actions
    void setUpRecentFileMenu();
    // Add specified file to recent files list
    void addRecentFile(const QString &filePath);
    // Update recent file menu
    void updateRecentFileMenu();

    /*
     * Shared Dialog Models
     */
    public:

    ModifyChargesModel *modifyChargesModel{nullptr};

    public Q_SLOTS:
    // File
    void on_FileNewAction_triggered(bool checked);
    void on_FileOpenAction_triggered(bool checked);
    void recentFileSelected();
    void on_FileLoadRestartPointAction_triggered(bool checked);
    void on_FileSaveRestartPointAction_triggered(bool checked);
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
    void on_SimulationSetRestartFileFrequencyAction_triggered(bool checked);
    void on_SimulationDataManagerAction_triggered(bool checked);
    void on_SimulationClearAdditionalPotentialsAction_triggered(bool checked);
    void on_SimulationClearModuleDataAction_triggered(bool checked);
    void on_SimulationReduceChargesSigFigsAction_triggered(bool checked);
    // Species
    void on_SpeciesCreateAtomicAction_triggered(bool checked);
    void on_SpeciesCreateDrawAction_triggered(bool checked);
    void on_SpeciesCreateFromExistingAction_triggered(bool checked);
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
    void on_SpeciesCopyChargesFromAtomTypesAction_triggered(bool checked);
    void on_SpeciesSetAtomTypeChargesFromSpeciesAction_triggered(bool checked);
    void on_SpeciesScaleChargesAction_triggered(bool checked);
    void on_SpeciesSmoothChargesAction_triggered(bool checked);
    void on_SpeciesReduceChargesSigFigsAction_triggered(bool checked);
    // Configuration
    void on_ConfigurationCreateAction_triggered(bool checked);
    void on_ConfigurationRenameAction_triggered(bool checked);
    void on_ConfigurationDeleteAction_triggered(bool checked);
    void on_ConfigurationExportToXYZAction_triggered(bool checked);
    void on_ConfigurationAdjustTemperatureAction_triggered(bool checked);
    // Layer
    void on_LayerCreateEmptyAction_triggered(bool checked);
    void on_LayerCreateEvolveBasicAtomicAction_triggered(bool checked);
    void on_LayerCreateEvolveAtomicAction_triggered(bool checked);
    void on_LayerCreateEvolveMolecularAction_triggered(bool checked);
    void on_LayerCreateEvolveMDAction_triggered(bool checked);
    void on_LayerCreateEvolveEPSRAction_triggered(bool checked);
    void on_LayerCreateRefineEPSRAction_triggered(bool checked);
    void on_LayerCreateCorrelationsRDFAction_triggered(bool checked);
    void on_LayerCreateCorrelationsRDFStructureFactorAction_triggered(bool checked);
    void on_LayerCreateCorrelationsRDFNeutronAction_triggered(bool checked);
    void on_LayerCreateCorrelationsRDFXRayAction_triggered(bool checked);
    void on_LayerCreateCorrelationsRDFNeutronXRayAction_triggered(bool checked);
    void on_LayerCreateAnalyseRDFCNAction_triggered(bool checked);
    void on_LayerCreateAnalyseAvgMolSDFAction_triggered(bool checked);
    void on_LayerRenameAction_triggered(bool checked);
    void on_LayerDeleteAction_triggered(bool checked);
    // Help
    void on_HelpOnlineManualAction_triggered(bool checked);
    void on_HelpOnlineTutorialsAction_triggered(bool checked);

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

    private:
    // Clear all module data
    bool clearModuleData(bool queryUser = true);
    // Prepare the simulation and run for a set count
    void setupIteration(int count);

    private Q_SLOTS:
    void on_MainTabs_currentChanged(int index);
    void statusLabelLinkClicked(const QString &link);

    public:
    // Return whether the GUI is currently running a simulation
    bool dissolveIterating() const;
    // Return list of all current tabs
    const std::vector<MainTab *> allTabs() const;

    public Q_SLOTS:
    // Disable editing
    void preventEditing();
    // Allow editing
    void allowEditing();
    // All iterations requested are complete
    void iterationsComplete();
    // Specified tab (indicated by page widget) has been closed, and relevant data should be deleted accordingly
    void closeTab(QWidget *page);

    Q_SIGNALS:
    void iterate(int);
    void stopIterating();
};

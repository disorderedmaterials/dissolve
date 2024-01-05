// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/messenger.h"
#include "gui/configurationTab.h"
#include "gui/gui.h"
#include "gui/layerTab.h"
#include "gui/mainTab.h"
#include "gui/selectRestartFileDialog.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <QCloseEvent>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <iostream>

// Default timer text
const QString defaultTimerText{"Idle"};

DissolveWindow::DissolveWindow(Dissolve &dissolve)
    : QMainWindow(nullptr), dissolve_(dissolve), recentFileLimit_(10), threadController_(this, dissolve)
{
    // Initialise resources
    Q_INIT_RESOURCE(main);

    // Set up user interface
    ui_.setupUi(this);

    // Set up the main tabs widget
    ui_.MainTabs->setUp(this);

    // Connect signals to thread controller
    connect(this, SIGNAL(iterate(int)), &threadController_, SLOT(iterate(int)));
    connect(this, SIGNAL(stopIterating()), &threadController_, SLOT(stopIterating()));

    // Connect signals from our main tab widget
    connect(ui_.MainTabs, SIGNAL(dataModified()), this, SLOT(setModified()));
    connect(ui_.MainTabs, SIGNAL(dataModified()), this, SLOT(fullUpdate()));
    connect(ui_.MainTabs, SIGNAL(tabClosed(QWidget *)), this, SLOT(closeTab(QWidget *)));

    refreshing_ = false;
    modified_ = false;
    dissolveIterating_ = false;
    elapsedTimer_.zero();

    // Create statusbar widgets
    addStatusBarIcon(":/general/icons/step.svg")->setToolTip("Current step / iteration number");
    iterationLabel_ = addStatusBarLabel("00000");
    iterationLabel_->setToolTip("Current step / iteration number");
    addStatusBarIcon(":/general/icons/clock.svg");
    timerLabel_ = addStatusBarLabel(defaultTimerText);
    restartFileIndicator_ = addStatusBarIcon(":/general/icons/restartfile.svg");
    statusIndicator_ = addStatusBarIcon(":/general/icons/true.svg", false);
    statusLabel_ = addStatusBarLabel("Unknown", false);
    statusLabel_->setOpenExternalLinks(false);
    connect(statusLabel_, SIGNAL(linkActivated(const QString)), this, SLOT(statusLabelLinkClicked(const QString)));

    // Create recent files menu
    setUpRecentFileMenu();

    updateWindowTitle();
    updateStatusBar();
    updateMenus();
}

// Catch window close event
void DissolveWindow::closeEvent(QCloseEvent *event)
{
    if (!checkSaveCurrentInput())
    {
        event->ignore();
        return;
    }

    // Save the state before we go...
    saveState();

    // If Dissolve is running, stop the thread now
    if (dissolveIterating_)
    {
        // Send the signal to stop
        emit(stopIterating());

        // Wait for the thread to stop
        while (dissolveIterating_)
            QApplication::processEvents();
    }

    // Clear tabs before we try to close down the application, otherwise we might get in to trouble with object deletion
    refreshing_ = true;
    ui_.MainTabs->clearTabs();
    ui_.MainTabs->clear();

    event->accept();
}

void DissolveWindow::resizeEvent(QResizeEvent *event) {}

/*
 * Dissolve Integration
 */

// Return reference to Dissolve
Dissolve &DissolveWindow::dissolve() { return dissolve_; }

const Dissolve &DissolveWindow::dissolve() const { return dissolve_; }

/*
 * Data Mutation Signalling
 */

// Flag that data has been modified via the GUI
void DissolveWindow::setModified(Flags<DissolveSignals::DataMutations> dataMutationFlags)
{
    modified_ = true;

    // Signal if any data was modified
    if (dataMutationFlags.anySet())
        emit(dataMutated(dataMutationFlags));

    updateWindowTitle();
}

/*
 * Statusbar
 */

// Add text label to status bar
QLabel *DissolveWindow::addStatusBarLabel(QString text, bool permanent)
{
    auto *label = new QLabel(text);
    if (permanent)
        statusBar()->addPermanentWidget(label);
    else
        statusBar()->addWidget(label);
    return label;
}

// Add text label to status bar
QLabel *DissolveWindow::addStatusBarIcon(QString resource, bool permanent)
{
    const auto iconSize = statusBar()->font().pointSize() * 1.75;
    auto *label = new QLabel;
    label->setPixmap(QPixmap(resource));
    label->setMaximumSize(QSize(iconSize, iconSize));
    label->setScaledContents(true);
    if (permanent)
        statusBar()->addPermanentWidget(label);
    else
        statusBar()->addWidget(label);
    return label;
}

/*
 * File
 */

// Load specified input file, and optionally handle restart file choice as well
bool DissolveWindow::loadInputFile(std::string_view inputFile, bool handleRestartFile)
{
    // Clear any current tabs
    refreshing_ = true;
    ui_.MainTabs->clearTabs();
    refreshing_ = false;

    // Clear Dissolve itself
    dissolve_.clear();

    // Clear the messages buffer
    clearMessages();

    // Set the current dir to the location of the new file
    QFileInfo inputFileInfo(QString::fromStdString(std::string(inputFile)));

    // Load the input file
    Messenger::banner("Parse Input File");
    auto loadResult = false;
    if (inputFileInfo.exists())
    {
        // Add file to recent menu
        addRecentFile(inputFileInfo.absoluteFilePath());
        QDir::setCurrent(inputFileInfo.absoluteDir().absolutePath());
        try
        {
            loadResult = dissolve_.loadInput(qPrintable(inputFileInfo.fileName()));
        }
        catch (...)
        {
            loadResult = false;
        }

        if (!loadResult)
        {
            QMessageBox::warning(this, "Input File Contained Errors",
                                 "The input file failed to load correctly.\nCheck the simulation carefully, and "
                                 "see the messages for more details.",
                                 QMessageBox::Ok, QMessageBox::Ok);

            // Forcibly show the main stack page so the user can see what happened (the input file name will remain unset)
            ui_.MainStack->setCurrentIndex(1);

            return false;
        }
    }
    else
    {
        // Show the splash page
        ui_.MainStack->setCurrentIndex(0);

        QMessageBox::warning(this, "Input File Not Found", "The specified input file does not exist.", QMessageBox::Ok,
                             QMessageBox::Ok);

        return false;
    }

    modified_ = false;
    dissolveIterating_ = false;

    Messenger::banner("Setting Up Processing Modules");

    if (!dissolve_.coreData().setUpProcessingLayerModules(dissolve_))
        return false;

    // Handle restart file loading?
    if (handleRestartFile)
    {
        fullUpdate();

        // Load / handle restart file
        SelectRestartFileDialog selectRestartFileDialog(this);
        auto restartFile = selectRestartFileDialog.getRestartFileName(inputFileInfo.filePath());
        if (!restartFile.isEmpty())
            loadRestartFile(restartFile.toStdString());

        fullUpdate();
    }

    // Empty timer text
    elapsedTimer_.zero();
    timerLabel_->setText(defaultTimerText);

    return true;
}

// Load specified restart file
bool DissolveWindow::loadRestartFile(std::string_view restartFile)
{
    Messenger::banner("Parse Restart File");

    auto loadSuccess = true;

    if (DissolveSys::fileExists(restartFile))
    {
        Messenger::print("Restart file '{}' exists and will be loaded.\n", restartFile);
        loadSuccess = dissolve_.loadRestart(restartFile);
    }
    else
        Messenger::print("Restart file '{}' does not exist.\n", restartFile);

    if (!loadSuccess)
        QMessageBox::warning(this, "Restart file contained errors.",
                             "The restart file failed to load correctly.\nSee the messages for more details.", QMessageBox::Ok,
                             QMessageBox::Ok);

    return loadSuccess;
}

/*
 * Update Functions
 */

// Update window title
void DissolveWindow::updateWindowTitle()
{
    // Window Title
    QString title =
        QString("Dissolve v%1 - %2%3")
            .arg(QString::fromStdString(std::string(Version::info())),
                 dissolve_.hasInputFilename() ? QString::fromStdString(std::string(dissolve_.inputFilename())) : "<untitled>",
                 modified_ ? "(*)" : "");
    setWindowTitle(title);

    // Update save menu item
    ui_.FileSaveAction->setEnabled(modified_);
}

// Update status bar
void DissolveWindow::updateStatusBar()
{
    // Set current iteration number
    iterationLabel_->setText(QStringLiteral("%1").arg(dissolve_.iteration(), 6, 10, QLatin1Char('0')));

    // Set restart file locations
    restartFileIndicator_->setEnabled(dissolve_.hasRestartFilename());
    restartFileIndicator_->setToolTip(dissolve_.hasRestartFilename()
                                          ? QStringLiteral("Current restart file is '%1'")
                                                .arg(QString::fromStdString(std::string(dissolve_.restartFilename())))
                                          : "No restart file available");

    // Set status
    if (Messenger::nErrors() > 0)
    {
        statusLabel_->setText(QString("%1 %2 (see <a href='Messages'>Messages</a>)")
                                  .arg(QString::number(Messenger::nErrors()), Messenger::nErrors() == 1 ? "Error" : "Errors"));
        statusIndicator_->setPixmap(QPixmap(":/general/icons/false.svg"));
    }
    else if (dissolveIterating_)
    {
        statusLabel_->setText("Running (ESC to stop)");
        statusIndicator_->setPixmap(QPixmap(":/general/icons/play.svg"));
    }
    else if (!dissolveIterating_ && elapsedTimer_.secondsElapsed() > 0.0)
    {
        statusLabel_->setText("Stopped");
        statusIndicator_->setPixmap(QPixmap(":/general/icons/true.svg"));
        timerLabel_->setText(QString::fromStdString(fmt::format("Time elapsed: {}", elapsedTimer_.elapsedTimeString(true))));
    }
    else if (ui_.MainStack->currentIndex() == 1)
    {
        statusLabel_->setText("Idle");
        statusIndicator_->setPixmap(QPixmap(":/general/icons/true.svg"));
    }
    else
    {
        statusLabel_->setText("No simulation loaded");
        statusIndicator_->setPixmap(QPixmap(":/dissolve/icons/dissolve.png"));
    }

    // Set restart file info
    restartFileIndicator_->setEnabled(dissolve_.hasRestartFilename());
    restartFileIndicator_->setToolTip(dissolve_.hasRestartFilename()
                                          ? QStringLiteral("Current restart file is '%1'")
                                                .arg(QString::fromStdString(std::string(dissolve_.restartFilename())))
                                          : "No restart file available");
}

// Update menus
void DissolveWindow::updateMenus()
{
    auto hasSimulation = ui_.MainStack->currentIndex() == 1;
    auto allowEditing = hasSimulation && !dissolveIterating_;

    // File Menu - always active, but available items depends on state
    ui_.FileSaveAction->setEnabled(hasSimulation && modified_);
    ui_.FileSaveAsAction->setEnabled(allowEditing);
    ui_.FileCloseAction->setEnabled(allowEditing);

    // Enable / disable other menu items as appropriate
    for (auto *action : ui_.SimulationMenu->actions())
        action->setEnabled((action == ui_.SimulationStopAction) == !allowEditing);
    ui_.SimulationMenu->setEnabled(hasSimulation);
    ui_.SpeciesMenu->setEnabled(allowEditing);
    ui_.ConfigurationMenu->setEnabled(allowEditing);
    ui_.LayerMenu->setEnabled(allowEditing);

    auto activeTab = ui_.MainTabs->currentTab();
    if (!activeTab)
        return;

    // Species Menu
    auto speciesAtomSelection = activeTab->type() == MainTab::TabType::Species ? ui_.MainTabs->currentSpecies()->selectedAtoms()
                                                                               : std::vector<const SpeciesAtom *>();
    ui_.SpeciesRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesAddForcefieldTermsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesReduceForcefieldTermsMenu->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesCopyTermsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesSetAtomTypesInSelectionAction->setEnabled(activeTab->type() == MainTab::TabType::Species &&
                                                         ui_.MainTabs->currentSpecies()->isSelectionSingleElement());
    ui_.SpeciesSetChargesInSelectionAction->setEnabled(activeTab->type() == MainTab::TabType::Species &&
                                                       !ui_.MainTabs->currentSpecies()->selectedAtoms().empty());
    ui_.SpeciesCopyChargesFromAtomTypesAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesSetAtomTypeChargesFromSpeciesAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesScaleChargesAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesSmoothChargesAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesReduceChargesSigFigsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesRegenerateIntraFromConnectivityAction->setEnabled(activeTab->type() == MainTab::TabType::Species);

    // Configuration Menu
    ui_.ConfigurationRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Configuration);
    ui_.ConfigurationDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Configuration);
    ui_.ConfigurationExportToMenu->setEnabled(activeTab->type() == MainTab::TabType::Configuration);
    ui_.ConfigurationAdjustTemperatureAction->setEnabled(activeTab->type() == MainTab::TabType::Configuration);

    // Layer Menu
    ui_.LayerRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Layer);
    ui_.LayerDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Layer);
}

// Perform full update of the GUI, including tab reconciliation
void DissolveWindow::fullUpdate()
{
    refreshing_ = true;

    // Move off the ident page if we currently have an input file (name)
    if (dissolve_.hasInputFilename())
        ui_.MainStack->setCurrentIndex(1);

    ui_.MainTabs->reconcileTabs();
    ui_.MainTabs->updateAllTabs();

    updateWindowTitle();
    updateStatusBar();
    updateMenus();

    refreshing_ = false;
}

// Update while running
void DissolveWindow::updateWhileRunning(int iterationsRemaining)
{

    refreshing_ = true;

    // Set current iteration number
    iterationLabel_->setText(QStringLiteral("%1").arg(dissolve_.iteration(), 6, 10, QLatin1Char('0')));
    // Text is set to time elapsed if iterating indefinitely
    if (iterationsRemaining == -1)
    {
        timerLabel_->setText(QString::fromStdString(fmt::format("Time elapsed: {}", elapsedTimer_.elapsedTimeString(true))));
    }
    // Set ETA text if we can
    else
    {
        auto estimatedTime = dissolve_.estimateRequiredTime(iterationsRemaining);
        timerLabel_->setText(estimatedTime ? QString::fromStdString(fmt::format(
                                                 "Time remaining: {}", Timer::timeString(estimatedTime.value(), false)))
                                           : defaultTimerText);
    }

    // Enable data access in Renderables, and update all tabs.
    Renderable::setSourceDataAccessEnabled(true);
    Renderable::validateAll(dissolve_.processingModuleData());
    ui_.MainTabs->updateAllTabs();
    repaint();
    Renderable::setSourceDataAccessEnabled(false);

    refreshing_ = false;
}

// Clear the messages window
void DissolveWindow::clearMessages()
{
    ui_.MainTabs->messagesTab()->clearMessages();
    Messenger::clearErrorCounts();
}

void DissolveWindow::statusLabelLinkClicked(const QString &link)
{
    if (DissolveSys::sameString(link.toStdString(), "Messages"))
        ui_.MainTabs->setCurrentIndex(0);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/messenger.h"
#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/workspacetab.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <QCloseEvent>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>
#include <iostream>

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

    // Create statusbar widgets
    addStatusBarIcon(":/control/icons/control_step.svg")->setToolTip("Current step / iteration number");
    iterationLabel_ = addStatusBarLabel("00000");
    iterationLabel_->setToolTip("Current step / iteration number");
    addStatusBarIcon(":/general/icons/general_clock.svg")->setToolTip("Time remaining to completion");
    etaLabel_ = addStatusBarLabel("--:--:--");
    etaLabel_->setToolTip("Time remaining to completion");
    restartFileIndicator_ = addStatusBarIcon(":/general/icons/general_restartfile.svg");
    statusIndicator_ = addStatusBarIcon(":/general/icons/general_true.svg", false);
    statusLabel_ = addStatusBarLabel("Unknown", false);

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

// Flag that data has been modified via the GUI
void DissolveWindow::setModified()
{
    modified_ = true;

    updateWindowTitle();
}

// Return reference to Dissolve
Dissolve &DissolveWindow::dissolve() { return dissolve_; }

const Dissolve &DissolveWindow::dissolve() const { return dissolve_; }

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

// Open specified input file from the CLI
bool DissolveWindow::openLocalFile(std::string_view inputFile, std::optional<std::string_view> restartFile,
                                   bool ignoreRestartFile)
{
    // Clear any current tabs
    refreshing_ = true;
    ui_.MainTabs->clearTabs();
    refreshing_ = false;

    // Clear Dissolve itself
    dissolve_.clear();

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
            QMessageBox::warning(this, "Input file contained errors.",
                                 "The input file failed to load correctly.\nCheck the simulation carefully, and "
                                 "see the messages for more details.",
                                 QMessageBox::Ok, QMessageBox::Ok);

            // Forcibly show the main stack page so the user can see what happened (the input file name will remain unset)
            ui_.MainStack->setCurrentIndex(1);

            return false;
        }
    }
    else
        return Messenger::error("Input file does not exist.\n");

    // Load restart file if it exists
    Messenger::banner("Parse Restart File");
    if (ignoreRestartFile)
        Messenger::print("Restart file (if it exists) will be ignored.\n");
    else
    {
        std::string actualRestartFile{restartFile.value_or(fmt::format("{}.restart", dissolve_.inputFilename()))};

        if (DissolveSys::fileExists(actualRestartFile))
        {
            Messenger::print("Restart file '{}' exists and will be loaded.\n", actualRestartFile);
            if (!dissolve_.loadRestart(actualRestartFile))
                QMessageBox::warning(this, "Restart file contained errors.",
                                     "The restart file failed to load correctly.\nSee the messages for more details.",
                                     QMessageBox::Ok, QMessageBox::Ok);

            // Reset the restart filename to be the standard one
            dissolve_.setRestartFilename(fmt::format("{}.restart", dissolve_.inputFilename()));
        }
        else
            Messenger::print("Restart file '{}' does not exist.\n", actualRestartFile);
    }

    modified_ = false;
    dissolveIterating_ = false;

    Messenger::banner("Setting Up Processing Modules");

    return dissolve_.setUpProcessingLayerModules();
}

/*
 * Open Recent Functions
 */
void DissolveWindow::addRecentFile(const QString &filePath)
{
    // Add new entry to recent files
    QSettings settings;
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    if (recentFilePaths.size() > recentFileLimit_)
        recentFilePaths.erase(recentFilePaths.begin() + recentFileLimit_, recentFilePaths.end());
    settings.setValue("recentFiles", recentFilePaths);
    updateRecentActionList();
}

void DissolveWindow::openRecent()
{
    if (!checkSaveCurrentInput())
        return;

    auto *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        std::string filePath = action->data().toString().toUtf8().constData();
        openLocalFile(filePath);

        fullUpdate();
    }
}

void DissolveWindow::createRecentMenu()
{
    QFont font = ui_.SessionMenu->font();
    ui_.FileOpenRecentMenu->setFont(font);

    for (auto i = 0; i < recentFileLimit_; ++i)
    {
        auto *recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        QObject::connect(recentFileAction, SIGNAL(triggered(bool)), this, SLOT(openRecent()));
        ui_.FileOpenRecentMenu->addAction(recentFileAction);
        recentFileActionList_.append(recentFileAction);
    }

    updateRecentActionList();
}

void DissolveWindow::updateRecentActionList()
{
    QSettings settings;
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();

    // Fill recent menu
    for (auto i = 0; i < recentFileLimit_; ++i)
    {
        if (i < recentFilePaths.size())
        {
            auto fileInfo = QFileInfo(recentFilePaths.at(i));
            recentFileActionList_.at(i)->setText(fileInfo.fileName() + "    (" + fileInfo.absoluteDir().absolutePath() + ")");
            recentFileActionList_.at(i)->setData(recentFilePaths.at(i));
            recentFileActionList_.at(i)->setVisible(true);
        }
        else
            recentFileActionList_.at(i)->setVisible(false);
    }
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
        statusLabel_->setText(QString("%1 %2 (see Messages)")
                                  .arg(QString::number(Messenger::nErrors()), Messenger::nErrors() == 1 ? "Error" : "Errors"));
        statusIndicator_->setPixmap(QPixmap(":/general/icons/general_false.svg"));
    }
    else if (dissolveIterating_)
    {
        statusLabel_->setText("Running (ESC to stop)");
        statusIndicator_->setPixmap(QPixmap(":/control/icons/control_play.svg"));
    }
    else
    {
        statusLabel_->setText("Idle");
        statusIndicator_->setPixmap(QPixmap(":/general/icons/general_true.svg"));
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
        action->setEnabled(action == ui_.SimulationStopAction == !allowEditing);
    ui_.SpeciesMenu->setEnabled(allowEditing);
    ui_.ConfigurationMenu->setEnabled(allowEditing);
    ui_.LayerMenu->setEnabled(allowEditing);
    ui_.WorkspaceMenu->setEnabled(allowEditing);

    auto activeTab = ui_.MainTabs->currentTab();
    if (!activeTab)
        return;

    // Species Menu
    auto speciesAtomSelection = activeTab->type() == MainTab::TabType::Species ? ui_.MainTabs->currentSpecies()->selectedAtoms()
                                                                               : std::vector<SpeciesAtom *>();
    ui_.SpeciesRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesAddForcefieldTermsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesReduceForcefieldTermsMenu->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesCopyTermsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesRegenerateIntraFromConnectivityAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesSetAtomTypesInSelectionAction->setEnabled(activeTab->type() == MainTab::TabType::Species &&
                                                         ui_.MainTabs->currentSpecies()->isSelectionSingleElement());
    ui_.SpeciesSetChargesInSelectionAction->setEnabled(activeTab->type() == MainTab::TabType::Species &&
                                                       !ui_.MainTabs->currentSpecies()->selectedAtoms().empty());

    // Configuration Menu
    ui_.ConfigurationRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Configuration);
    ui_.ConfigurationDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Configuration);
    ui_.ConfigurationExportToMenu->setEnabled(activeTab->type() == MainTab::TabType::Configuration);

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

    ui_.MainTabs->reconcileTabs(this);
    ui_.MainTabs->updateAllTabs();
    updateRecentActionList();
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

    // Set ETA text if we can
    if (iterationsRemaining == -1)
        etaLabel_->setText("--:--:--");
    else
    {
        auto estimatedTime = dissolve_.estimateRequiredTime(iterationsRemaining);
        etaLabel_->setText(estimatedTime ? QString::fromStdString(Timer::etaString(estimatedTime.value())) : "??:??:??");
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

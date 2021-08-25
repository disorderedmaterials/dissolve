// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/messenger.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <QCloseEvent>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFontDatabase>
#include <QLCDNumber>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>

DissolveWindow::DissolveWindow(Dissolve &dissolve)
    : QMainWindow(nullptr), dissolve_(dissolve), threadController_(this, dissolve), recentFileNo_(10)
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

    refreshing_ = false;
    modified_ = false;
    localSimulation_ = true;
    dissolveState_ = NoState;

    // Create statusbar widgets
    localSimulationIndicator_ = new QLabel;
    localSimulationIndicator_->setPixmap(QPixmap(":/general/icons/general_local.svg"));
    localSimulationIndicator_->setMaximumSize(QSize(20, 20));
    localSimulationIndicator_->setScaledContents(true);
    restartFileIndicator_ = new QLabel;
    restartFileIndicator_->setPixmap(QPixmap(":/general/icons/general_restartfile.svg"));
    restartFileIndicator_->setMaximumSize(QSize(20, 20));
    restartFileIndicator_->setScaledContents(true);
    heartbeatFileIndicator_ = new QLabel;
    heartbeatFileIndicator_->setPixmap(QPixmap(":/general/icons/general_heartbeat.svg"));
    heartbeatFileIndicator_->setMaximumSize(QSize(20, 20));
    heartbeatFileIndicator_->setScaledContents(true);
    iterationLabel_ = new QLabel("00000");
    etaLabel_ = new QLabel("ETA: --:--:--");

    statusBar()->addPermanentWidget(iterationLabel_);
    statusBar()->addPermanentWidget(etaLabel_);
    statusBar()->addPermanentWidget(heartbeatFileIndicator_);
    statusBar()->addPermanentWidget(restartFileIndicator_);
    statusBar()->addPermanentWidget(localSimulationIndicator_);

    updateWindowTitle();
    updateStatusBar();
    updateMenus();
}

// Catch window close event
void DissolveWindow::closeEvent(QCloseEvent *event)
{
    // Mark the window as refreshing, so we don't try to update any more widgets
    refreshing_ = true;

    if (!checkSaveCurrentInput())
    {
        event->ignore();
        return;
    }

    // Save the state before we go...
    saveState();

    // If Dissolve is running, stop the thread now
    if (dissolveState_ == RunningState)
    {
        // Send the signal to stop
        emit(stopIterating());

        // Wait for the thread to stop
        while (dissolveState_ == RunningState)
            QApplication::processEvents();
    }

    // Clear tabs before we try to close down the application, otherwise we'll get in to trouble with object deletion
    ui_.MainTabs->clearTabs();

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
 * File
 */

// Open specified input file from the CLI
bool DissolveWindow::openLocalFile(std::string_view inputFile, std::string_view restartFile, bool ignoreRestartFile,
                                   bool ignoreLayoutFile)
{
    refreshing_ = true;

    // Clear any existing tabs etc.
    ui_.MainTabs->clearTabs();

    // Clear Dissolve itself
    dissolve_.clear();

    // Set the current dir to the location of the new file
    QFileInfo inputFileInfo(QString::fromStdString(std::string(inputFile)));

    // Load the input file
    Messenger::banner("Parse Input File");
    auto loadResult = false;
    if (inputFileInfo.exists())
    {
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
            QMessageBox::warning(this, "Input file contained errors.",
                                 "The input file failed to load correctly.\nCheck the simulation carefully, and "
                                 "see the messages for more details.",
                                 QMessageBox::Ok, QMessageBox::Ok);
    }
    else
        return Messenger::error("Input file does not exist.\n");

    // Load restart file if it exists
    Messenger::banner("Parse Restart File");
    if (!ignoreRestartFile)
    {
        std::string actualRestartFile{restartFile};
        if (actualRestartFile.empty())
            actualRestartFile = fmt::format("{}.restart", dissolve_.inputFilename());

        if (DissolveSys::fileExists(actualRestartFile))
        {
            Messenger::print("\nRestart file '{}' exists and will be loaded.\n", actualRestartFile);
            if (!dissolve_.loadRestart(actualRestartFile))
                QMessageBox::warning(this, "Restart file contained errors.",
                                     "The restart file failed to load correctly.\nSee the messages for more details.",
                                     QMessageBox::Ok, QMessageBox::Ok);

            // Reset the restart filename to be the standard one
            dissolve_.setRestartFilename(fmt::format("{}.restart", dissolve_.inputFilename()));
        }
        else
            Messenger::print("\nRestart file '{}' does not exist.\n", actualRestartFile);
    }
    else
        Messenger::print("\nRestart file (if it exists) will be ignored.\n");

    refreshing_ = true;

    ui_.MainTabs->reconcileTabs(this);

    refreshing_ = false;

    // Does a window state exist for this input file?
    stateFilename_ = QStringLiteral("%1.state").arg(QString::fromStdString(std::string(dissolve_.inputFilename())));

    // Try to load in the window state file
    if (QFile::exists(stateFilename_) && (!ignoreLayoutFile))
        loadState();

    localSimulation_ = true;

    // Check the beat file
    QString beatFile = QStringLiteral("%1.beat").arg(QString::fromStdString(std::string(dissolve_.inputFilename())));
    if (QFile::exists(beatFile))
    {
        // TODO
        // 		if (
    }

    dissolveState_ = EditingState;
    // Add file to recent menu
    QString filePath = inputFileInfo.absoluteDir().absolutePath();
    std::string fileName = dissolve_.inputFilename().data();
    addRecentFile(filePath + QString::fromStdString("/" + fileName));
    // Fully update GUI
    fullUpdate();

    return true;
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
    if (recentFilePaths.size() > recentFileNo_)
        recentFilePaths.erase(recentFilePaths.begin() + recentFileNo_, recentFilePaths.end());
    settings.setValue("recentFiles", recentFilePaths);
    updateRecentActionList();
}

void DissolveWindow::openRecent()
{
    if (!checkSaveCurrentInput())
        return;

    std::string filePath = "";
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        filePath = action->data().toString().toUtf8().constData();
        openLocalFile(filePath, "", false, false);
    }
}

void DissolveWindow::createRecentMenu()
{
    int recentLength = recentFileNo_;
    QAction *recentFileAction = 0;

    QFont font = ui_.SessionMenu->font();
    ui_.FileOpenRecentMenu->setFont(font);

    for (auto i = 0; i < recentLength; i++)
    {
        recentFileAction = new QAction(this);
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
    for (auto i = 0u; i < recentFileNo_; ++i)
    {
        if (i < recentFilePaths.size())
        {
            QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
            recentFileActionList_.at(i)->setText(strippedName + "    " + recentFilePaths.at(i));
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

    // Set relevant file locations
    if (localSimulation_)
    {
        localSimulationIndicator_->setPixmap(QPixmap(":/general/icons/general_local.svg"));
        restartFileIndicator_->setEnabled(dissolve_.hasRestartFilename());
        restartFileIndicator_->setToolTip(dissolve_.hasRestartFilename()
                                              ? QStringLiteral("Current restart file is '%1'")
                                                    .arg(QString::fromStdString(std::string(dissolve_.restartFilename())))
                                              : "No restart file available");
        heartbeatFileIndicator_->setEnabled(false);
        heartbeatFileIndicator_->setToolTip("Heartbeat file not monitored.");
    }
    else
    {
        localSimulationIndicator_->setPixmap(QPixmap(":/menu/icons/menu_connect.svg"));
        // TODO!
    }
}

// Update menus
void DissolveWindow::updateMenus()
{
    // File Menu - always active, but available items depends on state
    ui_.FileSaveAction->setEnabled(dissolveState_ != NoState);
    ui_.FileSaveAsAction->setEnabled(dissolveState_ != NoState);
    ui_.FileCloseAction->setEnabled(dissolveState_ != NoState && dissolveState_ != RunningState);

    // Enable / disable other menu items as appropriate
    ui_.SimulationMenu->setEnabled(dissolveState_ == EditingState);
    ui_.SpeciesMenu->setEnabled(dissolveState_ == EditingState);
    ui_.ConfigurationMenu->setEnabled(dissolveState_ == EditingState);
    ui_.LayerMenu->setEnabled(dissolveState_ == EditingState);
    ui_.WorkspaceMenu->setEnabled(dissolveState_ == EditingState);

    auto activeTab = ui_.MainTabs->currentTab();
    if (!activeTab)
        return;

    // Species Menu
    ui_.SpeciesRenameAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesDeleteAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesAddForcefieldTermsAction->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesReduceForcefieldTermsMenu->setEnabled(activeTab->type() == MainTab::TabType::Species);
    ui_.SpeciesRegenerateIntraFromConnectivityAction->setEnabled(activeTab->type() == MainTab::TabType::Species);

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

    // Make sure correct stack page is shown
    ui_.MainStack->setCurrentIndex(dissolveState_ == NoState ? 0 : 1);
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
        etaLabel_->setText("ETA: --:--:--");
    else
        etaLabel_->setText(QStringLiteral("ETA: %1").arg(
            QString::fromStdString(Timer::etaString(iterationsRemaining * dissolve_.iterationTime()))));

    // Enable data access in Renderables, and update all tabs.
    Renderable::setSourceDataAccessEnabled(true);
    Renderable::validateAll(dissolve_.processingModuleData());
    ui_.MainTabs->updateAllTabs();
    repaint();
    Renderable::setSourceDataAccessEnabled(false);

    refreshing_ = false;
}

// Clear the messages window
void DissolveWindow::clearMessages() { ui_.MainTabs->messagesTab()->clearMessages(); }

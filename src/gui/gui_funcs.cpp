/*
	*** Dissolve GUI - Core Functions
	*** src/gui/gui_funcs.cpp
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

#include "main/dissolve.h"
#include "version.h"
#include "gui/gui.h"
#include "gui/configurationtab.h"
#include "gui/forcefieldtab.h"
#include "gui/moduletab.h"
#include "gui/modulelayertab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"
#include "base/lineparser.h"
#include <QCloseEvent>
#include <QDir>
#include <QDirIterator>
#include <QFontDatabase>
#include <QLCDNumber>
#include <QMdiSubWindow>
#include <QFileInfo>

// Constructor
DissolveWindow::DissolveWindow(Dissolve& dissolve) : QMainWindow(NULL), dissolve_(dissolve), threadController_(this, dissolve)
{
	// Initialise resources
	Q_INIT_RESOURCE(main);

	// Register our custom font (for the GuideWizard)
	QFontDatabase::addApplicationFont(":/fonts/fonts/SourceSansPro-Regular.ttf");

	// Set up user interface
	ui.setupUi(this);

	// Connect signals to thread controller
	connect(this, SIGNAL(iterate(int)), &threadController_, SLOT(iterate(int)));
	connect(this, SIGNAL(stopIterating()), &threadController_, SLOT(stopIterating()));

	// Connect signals from our main tab widget / bar
	connect(ui.MainTabs, SIGNAL(tabClosed(QWidget*)), this, SLOT(removeTab(QWidget*)));
	connect(ui.MainTabs, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(mainTabsDoubleClicked(int)));
	dissolveState_ = EditingState;

	refreshing_ = false;
	modified_ = false;
	localSimulation_ = true;

	// Create statusbar widgets
	localSimulationIndicator_ = new QLabel;
	localSimulationIndicator_->setPixmap(QPixmap(":/general/icons/general_local.svg"));
	localSimulationIndicator_->setMaximumSize(QSize(20,20));
	localSimulationIndicator_->setScaledContents(true);
	restartFileIndicator_ = new QLabel;
	restartFileIndicator_->setPixmap(QPixmap(":/general/icons/general_restartfile.svg"));
	restartFileIndicator_->setMaximumSize(QSize(20,20));
	restartFileIndicator_->setScaledContents(true);
	heartbeatFileIndicator_ = new QLabel;
	heartbeatFileIndicator_->setPixmap(QPixmap(":/general/icons/general_heartbeat.svg"));
	heartbeatFileIndicator_->setMaximumSize(QSize(20,20));
	heartbeatFileIndicator_->setScaledContents(true);
	statusBar()->addPermanentWidget(heartbeatFileIndicator_);
	statusBar()->addPermanentWidget(restartFileIndicator_);
	statusBar()->addPermanentWidget(localSimulationIndicator_);

	addCoreTabs();

	updateTabs();
	updateWindowTitle();
	updateControlsFrame();

	// Initialise the available system templates
	initialiseSystemTemplates();

	// Show the Start stack page (we call this mostly to ensure correct availability of other controls)
	showMainStackPage(DissolveWindow::StartStackPage);
}

// Destructor
DissolveWindow::~DissolveWindow()
{
}

// Catch window close event
void DissolveWindow::closeEvent(QCloseEvent* event)
{
	// Mark the window as refreshing, so we don't try to update any more widgets
	refreshing_ = true;

	if (!checkSaveCurrentInput())
	{
		event->ignore();
		return;
	}

	// Save the state before we go...
	saveWindowLayout();

	event->accept();
}

void DissolveWindow::resizeEvent(QResizeEvent* event)
{
}

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
Dissolve& DissolveWindow::dissolve()
{
	return dissolve_;
}

// Return const reference to Dissolve
const Dissolve& DissolveWindow::constDissolve() const
{
	return dissolve_;
}

// Link output handler in to the Messenger
void DissolveWindow::addOutputHandler()
{
	Messenger::setOutputHandler(&outputHandler_);
	connect(&outputHandler_, SIGNAL(printText(const QString&)), ui.MessagesBrowser, SLOT(append(const QString&)));
}

/*
 * File
 */

// Open specified input file from the CLI
bool DissolveWindow::openFileFromCLI(const char* inputFile, const char* restartFile, bool ignoreRestartFile, bool ignoreLayoutFile)
{
	// Clear any existing tabs etc.
	clearTabs();

	// Clear Dissolve itself
	dissolve_.clear();

	// Load the input file
	Messenger::banner("Parse Input File");
	if (!dissolve_.loadInput(inputFile))
	{
		dissolve_.clear();
		return false;
	}

	// Load restart file if it exists
	Messenger::banner("Parse Restart File");
	if (!ignoreRestartFile)
	{
		CharString actualRestartFile = restartFile;
		if (actualRestartFile.isEmpty()) actualRestartFile.sprintf("%s.restart", inputFile);
		
		if (DissolveSys::fileExists(actualRestartFile))
		{
			Messenger::print("\nRestart file '%s' exists and will be loaded.\n", actualRestartFile.get());
			if (!dissolve_.loadRestart(actualRestartFile))
			{
				Messenger::error("Restart file contained errors.\n");
				ProcessPool::finalise();
				return 1;
			}

			// Reset the restart filename to be the standard one
			dissolve_.setRestartFilename(CharString("%s.restart", inputFile));
		}
		else Messenger::print("\nRestart file '%s' does not exist.\n", actualRestartFile.get());
	}
	else Messenger::print("\nRestart file (if it exists) will be ignored.\n");

	refreshing_ = true;

	reconcileTabs();

	refreshing_ = false;

	// Does a window state exist for this input file?
	windowLayoutFilename_.sprintf("%s.state", dissolve_.inputFilename());

	// Try to load in the window state file
	if (DissolveSys::fileExists(windowLayoutFilename_) && (!ignoreLayoutFile)) loadWindowLayout();

	// Switch to the Simulation stack page
	showMainStackPage(DissolveWindow::SimulationStackPage);

	// Fully update GUI
	fullUpdate();

	return true;
}

/*
 * System Templates
 */

// Initialise system templates from the main resource
void DissolveWindow::initialiseSystemTemplates()
{
	// Probe our main resource object for the templates, and create local data from them
	QDirIterator templateIterator(":/data/systemtemplates/");
	while (templateIterator.hasNext())
	{
		QDir dir = templateIterator.next();

		// Open the associated xml file
		SystemTemplate* sysTemp = systemTemplates_.add();
		if (!sysTemp->read(dir))
		{
			Messenger::error("Error reading the template info file '%s'.\n", qPrintable(dir.filePath("info.xml")));
			systemTemplates_.remove(sysTemp);
			continue;
		}
	}
}

/*
 * Update Functions
 */

// Update all tabs
void DissolveWindow::updateTabs()
{
	RefList<MainTab> tabs = allTabs();
	RefListIterator<MainTab> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->updateControls();
}

// Update window title
void DissolveWindow::updateWindowTitle()
{
	// Window Title
	QString title = QString("Dissolve v%1 - %2%3").arg(DISSOLVEVERSION, dissolve_.hasInputFilename() ? dissolve_.inputFilename() : "<untitled>", modified_ ? "(*)" : "");
	setWindowTitle(title);
}

// Update controls frame
void DissolveWindow::updateControlsFrame()
{
	// Update ControlFrame to reflect Dissolve's current state
	ui.ControlRunButton->setEnabled(dissolveState_ == DissolveWindow::EditingState);
	ui.ControlStepButton->setEnabled(dissolveState_ == DissolveWindow::EditingState);
	ui.ControlPauseButton->setEnabled(dissolveState_ == DissolveWindow::RunningState);
	ui.ControlReloadButton->setEnabled(dissolveState_ == DissolveWindow::MonitoringState);

	// Set current iteration number
	ui.ControlIterationLabel->setText(CharString("%06i", dissolve_.iteration()).get());

	// Set relevant file locations
	if (localSimulation_)
	{
		localSimulationIndicator_->setPixmap(QPixmap(":/general/icons/general_local.svg"));
		restartFileIndicator_->setEnabled(dissolve_.hasRestartFilename());
		restartFileIndicator_->setToolTip(dissolve_.hasRestartFilename() ? CharString("Current restart file is '%s'", dissolve_.restartFilename()).get() : "No restart file available");
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
	MainTab* activeTab = currentTab();
	if (!activeTab) return;

	// Species Menu
	ui.SpeciesRenameAction->setEnabled(activeTab->type() == MainTab::SpeciesTabType);
	ui.SpeciesAddForcefieldTermsAction->setEnabled(activeTab->type() == MainTab::SpeciesTabType);
}

// Perform full update of the GUI, including tab reconciliation
void DissolveWindow::fullUpdate()
{
	reconcileTabs();

	updateTabs();
	updateWindowTitle();
	updateControlsFrame();
	updateMenus();
}

/*
 * Stack
 */

// Set currently-visible main stack page
void DissolveWindow::showMainStackPage(DissolveWindow::MainStackPage page)
{
	ui.MainStack->setCurrentIndex(page);

	// Enable / disable main menu items as appropriate
	ui.SimulationMenu->setEnabled(page == DissolveWindow::SimulationStackPage);
	ui.SpeciesMenu->setEnabled(page == DissolveWindow::SimulationStackPage);
	ui.ConfigurationMenu->setEnabled(page == DissolveWindow::SimulationStackPage);
	ui.LayerMenu->setEnabled(page == DissolveWindow::SimulationStackPage);
	ui.WorkspaceMenu->setEnabled(page == DissolveWindow::SimulationStackPage);
}

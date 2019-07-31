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
#include <QFontDatabase>
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

	// Set up GuideWidget
	ui.GuideWidget->setMainDissolveReference(&dissolve);
	ui.GuideWidget->setDissolveWindow(this);
	connect(ui.GuideWidget, SIGNAL(canceled()), this, SLOT(guideWidgetCanceled()));
	connect(ui.GuideWidget, SIGNAL(finished()), this, SLOT(guideWidgetFinished()));

	refreshing_ = false;
	modified_ = false;
	localSimulation_ = true;

	addCoreTabs();

	updateTabs();
	updateWindowTitle();
	updateControlsFrame();

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

// Flag that data has been modified via the GUI, and whether this invalidates the current setup
void DissolveWindow::setModified(bool invalidatesSetUp)
{
	modified_ = true;

	if (invalidatesSetUp) dissolve_.invalidateSetUp();

	updateWindowTitle();
}

// Flag that data has been modified via the GUI, and that the set up is now invalid
void DissolveWindow::setModifiedAndInvalidated()
{
	modified_ = true;

	dissolve_.invalidateSetUp();

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

	// Try to set-up simulation
	dissolve_.setUp();

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
	if (dissolveState_ == DissolveWindow::EditingState)
	{
		ui.ControlStateIcon->setPixmap(QPixmap(":/general/icons/general_edit.svg"));
		ui.ControlStateLabel->setText("EDITING");
		ui.ControlSetUpButton->setEnabled(!dissolve_.isSetUp());
		ui.ControlSetUpButton->setIcon(dissolve_.isSetUp() ? QIcon(":/general/icons/general_true.svg") : QIcon(":/general/icons/general_false.svg"));
		ui.ControlRunButton->setEnabled(dissolve_.isSetUp());
		ui.ControlPauseButton->setEnabled(false);
		ui.ControlReloadButton->setEnabled(false);
	}
	else if (dissolveState_ == DissolveWindow::RunningState)
	{
		ui.ControlStateIcon->setPixmap(QPixmap(":/general/icons/general_edit.svg"));
		ui.ControlStateLabel->setText("RUNNING");
		ui.ControlSetUpButton->setEnabled(false);
		ui.ControlSetUpButton->setIcon(QIcon(":/general/icons/general_true.svg"));
		ui.ControlRunButton->setEnabled(false);
		ui.ControlPauseButton->setEnabled(true);
		ui.ControlReloadButton->setEnabled(false);
	}
	else if (dissolveState_ == DissolveWindow::MonitoringState)
	{
		ui.ControlStateIcon->setPixmap(QPixmap(":/general/icons/general_monitor.svg"));
		ui.ControlStateLabel->setText("MONITOR");
		ui.ControlSetUpButton->setEnabled(false);
		ui.ControlSetUpButton->setIcon(QIcon(":/general/icons/general_true.svg"));
		ui.ControlRunButton->setEnabled(false);
		ui.ControlPauseButton->setEnabled(false);
		ui.ControlReloadButton->setEnabled(true);
	}

	// Set current iteration number
	ui.ControlIterationNumberLCD->display(DissolveSys::itoa(dissolve_.iteration()));

	// Set relevant file locations
	if (localSimulation_)
	{
		if (dissolve_.hasInputFilename())
		{
			ui.ControlLocationLabel->setText(QFileInfo(dissolve_.inputFilename()).absolutePath() + "  (Local)");
			ui.ControlInputFileLabel->setText(dissolve_.inputFilename());
		}
		else
		{
			ui.ControlLocationLabel->setText(QDir::current().absolutePath() + " (Local)");
			ui.ControlInputFileLabel->setText("<untitled>");
		}
		ui.ControlRestartFileLabel->setText(dissolve_.hasRestartFilename() ? dissolve_.restartFilename() : "<none>");
	}
	else
	{
		ui.ControlLocationLabel->setText("UNKNOWN NETWORK LOCATION");
		ui.ControlInputFileLabel->setText(dissolve_.inputFilename());
		ui.ControlRestartFileLabel->setText(dissolve_.hasRestartFilename() ? dissolve_.restartFilename() : "<none>");
	}
}

// Perform full update of the GUI, including tab reconciliation
void DissolveWindow::fullUpdate()
{
	reconcileTabs();

	updateTabs();
	updateWindowTitle();
	updateControlsFrame();
}

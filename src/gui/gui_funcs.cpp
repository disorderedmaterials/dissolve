/*
	*** Dissolve GUI - Core Functions
	*** src/gui/gui_funcs.cpp
	Copyright T. Youngs 2012-2018

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
#include "gui/processingtab.h"
#include "gui/speciestab.h"
#include "gui/workspacetab.h"
#include "base/lineparser.h"
#include <QCloseEvent>
#include <QDir>
#include <QMdiSubWindow>
#include <QFileInfo>

// Constructor
DissolveWindow::DissolveWindow(Dissolve& dissolve) : QMainWindow(NULL), dissolve_(dissolve), threadController_(this, dissolve)
{
	// Initialise resources
	Q_INIT_RESOURCE(main);
	Q_INIT_RESOURCE(uchroma);

	// Set up user interface
	ui.setupUi(this);

	// Connect signals to thread controller
	connect(this, SIGNAL(iterate(int)), &threadController_, SLOT(iterate(int)));
	connect(this, SIGNAL(stopIterating()), &threadController_, SLOT(stopIterating()));

	// Connect signals from our main tab widget / bar
	connect(ui.MainTabs, SIGNAL(tabClosed(QWidget*)), this, SLOT(removeTab(QWidget*)));
	connect(ui.MainTabs, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(mainTabsDoubleClicked(int)));
	dissolveState_ = StoppedState;

	// Set up GuideWidget
	ui.GuideWidget->setMainDissolveReference(&dissolve);
	ui.GuideWidget->setDissolveWindow(this);
	connect(ui.GuideWidget, SIGNAL(canceled()), this, SLOT(guideWidgetCanceled()));
	connect(ui.GuideWidget, SIGNAL(finished()), this, SLOT(guideWidgetFinished()));

	refreshing_ = false;
	modified_ = false;

	addCoreTabs();

	updateStatus();

	// Show the Start stack page (we call this mostly to ensure correct availability of other controls)
	showMainStackPage(DissolveWindow::StartStackPage);
}

// Destructor
DissolveWindow::~DissolveWindow()
{
}

// Flag that data has been modified via the GUI
void DissolveWindow::setModified()
{
	modified_ = true;

	updateStatus();
}

// Flag that data has been modified via the GUI, and that the set up is now invalid
void DissolveWindow::setModifiedAndInvalidated()
{
	modified_ = true;

	dissolve_.invalidateSetUp();

	updateStatus();
}

// Return reference to Dissolve
Dissolve& DissolveWindow::dissolve()
{
	return dissolve_;
}

// Catch window close event
void DissolveWindow::closeEvent(QCloseEvent* event)
{
	// Mark the window as refreshing, so we don't try to update any more widgets
	refreshing_ = true;

	// Save the state before we go...
	saveWindowLayout();

	event->accept();
}

void DissolveWindow::resizeEvent(QResizeEvent* event)
{
}

/*
 * File
 */

bool DissolveWindow::openFile(const char* inputFile, bool ignoreRestartFile, bool ignoreLayoutFile)
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
		CharString restartFile("%s.restart", inputFile);
		if (DissolveSys::fileExists(restartFile))
		{
			Messenger::print("\nRestart file '%s' exists and will be loaded.\n", restartFile.get());
			if (!dissolve_.loadRestart(restartFile.get()))
			{
				Messenger::error("Restart file contained errors.\n");
				ProcessPool::finalise();
				return 1;
			}
		}
		else Messenger::print("\nRestart file '%s' does not exist.\n", restartFile.get());
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

	updateControls();

	updateStatus();

	return true;
}

/*
 * Update Functions
 */

// Refresh all displayed widgets
void DissolveWindow::updateControls()
{
	// Iteration Panel
	ui.IterationNumberLabel->setText(DissolveSys::itoa(dissolve_.iteration()));

	// Update all tabs
	RefList<MainTab,bool> tabs = allTabs();
	RefListIterator<MainTab,bool> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate()) tab->updateControls();
}

// Update status
void DissolveWindow::updateStatus()
{
	// Window Title
	QString title = QString("Dissolve v%1 - %2%3").arg(DISSOLVEVERSION, dissolve_.hasInputFilename() ? dissolve_.inputFilename() : "<untitled>", modified_ ? "(*)" : "");
	setWindowTitle(title);

	// Dissolve Status
	QPalette labelPalette = ui.SetUpLabel->palette();
	if (dissolve_.isSetUp())
	{
		labelPalette.setColor(QPalette::WindowText, Qt::darkGreen);
		ui.SetUpLabel->setText("Set Up & Ready");
		ui.ControlSetUpButton->setEnabled(false);
	}
	else
	{
		labelPalette.setColor(QPalette::WindowText, Qt::darkRed);
		ui.SetUpLabel->setText("Not Set Up");
		ui.ControlSetUpButton->setEnabled(true);
	}
	ui.SetUpLabel->setPalette(labelPalette);
}

// Update file labels
void DissolveWindow::updateFileLabels()
{
	if (dissolve_.hasInputFilename())
	{
		ui.LocationLabel->setText(QFileInfo(dissolve_.inputFilename()).absolutePath() + "  (Local)");
		ui.InputFileLabel->setText(dissolve_.inputFilename());
	}
	else
	{
		ui.LocationLabel->setText(QDir::current().absolutePath() + " (Local)");
		ui.InputFileLabel->setText("<untitled>");
	}
	ui.RestartFileLabel->setText(dissolve_.hasRestartFilename() ? dissolve_.restartFilename() : "<none>");
}

// Link output handler in to the Messenger
void DissolveWindow::addOutputHandler()
{
	Messenger::setOutputHandler(&outputHandler_);
	connect(&outputHandler_, SIGNAL(printText(const QString&)), ui.MessagesBrowser, SLOT(append(const QString&)));
}

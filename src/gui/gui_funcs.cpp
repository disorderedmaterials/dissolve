/*
	*** Dissolve Main Window - Functions
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
#include "base/lineparser.h"
#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QFileInfo>

// Constructor
DissolveWindow::DissolveWindow(Dissolve& dissolve) : QMainWindow(NULL), dissolve_(dissolve), threadController_(this, dissolve)
{
	// Initialise resources
	Q_INIT_RESOURCE(icons);
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

	refreshing_ = false;
	modified_ = false;

	updateStatus();
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
	clearAllTabs();

	// Clear dissolve itself
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

	// Add on necessary tabs
	addCoreTabs();
	ListIterator<Configuration> configIterator(dissolve_.configurations());
	while (Configuration* cfg = configIterator.iterate()) addConfigurationTab(cfg);
	refreshing_ = false;

	// Does a window state exist for this input file?
	windowLayoutFilename_.sprintf("%s.state", dissolve_.inputFilename());

	// Try to load in the window state file
	if (DissolveSys::fileExists(windowLayoutFilename_) && (!ignoreLayoutFile)) loadWindowLayout();

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

	// Loop over tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next) tab->updateControls();
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
	ui.LocationLabel->setText(QFileInfo(dissolve_.inputFilename()).absolutePath() + "  (Local)");
	ui.InputFileLabel->setText(dissolve_.inputFilename());
	ui.RestartFileLabel->setText(dissolve_.hasRestartFilename() ? dissolve_.restartFilename() : "<none>");
}

// Link output handler in to the Messenger
void DissolveWindow::addOutputHandler()
{
	Messenger::setOutputHandler(&outputHandler_);
	connect(&outputHandler_, SIGNAL(printText(const QString&)), ui.MessagesBrowser, SLOT(append(const QString&)));
}

/*
 * Window State
 */

// Save current window layout
bool DissolveWindow::saveWindowLayout()
{
	// Open file for writing
	LineParser stateParser;
	stateParser.openOutput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForWriting()) return false;

	// Write current tab index
	if (!stateParser.writeLineF("%i\n", ui.MainTabs->currentIndex())) return false;

	// Loop over tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next)
	{
		// Write tab type and title
		if (!stateParser.writeLineF("'%s'  %s\n", tab->title(), tab->tabType())) return false;
		if (!tab->writeState(stateParser)) return false;
	}

	stateParser.closeFiles();

	return true;
}

// Load window layout
bool DissolveWindow::loadWindowLayout()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	// Read current tab index - it may not yet exist, so store it now and set it later
	if (stateParser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;
	int currentTab = stateParser.argi(0);

	// Remainder of file references tab types. Core tabs and those for Configurations will exist already. Others must be created.
	while (!stateParser.eofOrBlank())
	{
		// Parse the line, which contains the title and type of the tab
		if (stateParser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;

		// If any of our current tabs match the title, call it's readState() function
		MainTab* tab = findTab(stateParser.argc(0));
		if (tab)
		{
			if (!tab->readState(stateParser)) return false;
		}
		else
		{
			// Must first create the tab first.
			if (DissolveSys::sameString(stateParser.argc(1), "ModuleTab"))
			{
				// The title represents the unique name of the Module, so find it now
				Module* module = ModuleList::findInstanceByUniqueName(stateParser.argc(0));
				if (!module) return Messenger::error("Failed to find Module '%s' for display in a ModuleTab.\n", stateParser.argc(0)); 

				tab = addModuleTab(module);
				
			}
			else if (DissolveSys::sameString(stateParser.argc(1), "WorkspaceTab"))
			{
				// Create a new workspace with the desired name
				tab = addWorkspaceTab(stateParser.argc(0));
			}
			else return Messenger::error("Unrecognised tab type '%s' in state file.\n", stateParser.argc(1));

			// Now read state information
			if (!tab->readState(stateParser)) return false;
		}
	}

	// Set current tab (we stored the index earlier)
	ui.MainTabs->setCurrentIndex(currentTab);

	return true;
}

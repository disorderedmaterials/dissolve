/*
	*** dUQ Main Window - Functions
	*** src/gui/gui_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/duq.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/pairpotentialwidget.h"
#include "base/lineparser.h"
#include <QCloseEvent>
#include <QMdiSubWindow>

// Constructor
DUQWindow::DUQWindow(DUQ& duq) : QMainWindow(NULL), duq_(duq), threadController_(this, duq)
{
	// Initialise resources
	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(uchroma);

	// Set up user interface
	ui.setupUi(this);

	// Connect signals to thread controller
	connect(this, SIGNAL(iterate(int)), &threadController_, SLOT(iterate(int)));
	connect(this, SIGNAL(stopIterating()), &threadController_, SLOT(stopIterating()));

	// Connect signals from our main tabbar
	connect(ui.MainTabs, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(mainTabsDoubleClicked(int)));
	duqState_ = StoppedState;

	refreshing_ = false;
	modified_ = false;

	updateStatus();
}

// Destructor
DUQWindow::~DUQWindow()
{
}

// Flag that data has been modified via the GUI
void DUQWindow::setModified()
{
	modified_ = true;

	updateStatus();
}

// Return reference to dUQ
DUQ& DUQWindow::duq()
{
	return duq_;
}

// Catch window close event
void DUQWindow::closeEvent(QCloseEvent* event)
{
	// Mark the window as refreshing, so we don't try to update any more widgets
	refreshing_ = true;

	// Save the state before we go...
	saveWindowLayout();

	event->accept();
}

void DUQWindow::resizeEvent(QResizeEvent* event)
{
}

/*
 * File
 */

bool DUQWindow::openFile(const char* inputFile, bool ignoreRestartFile, bool ignoreLayoutFile)
{
	// Clear any existing tabs etc.
	clearAllTabs();

	// Clear duq itself
	duq_.clear();

	// Load the input file
	Messenger::banner("Parse Input File");
	if (!duq_.loadInput(inputFile))
	{
		Messenger::error("Input file contained errors.\n");
		duq_.clear();
		return false;
	}

	// Load restart file if it exists
	Messenger::banner("Parse Restart File");
	if (!ignoreRestartFile)
	{
		CharString restartFile("%s.restart", inputFile);
		if (DUQSys::fileExists(restartFile))
		{
			Messenger::print("\nRestart file '%s' exists and will be loaded.\n", restartFile.get());
			if (!duq_.loadRestart(restartFile.get()))
			{
				Messenger::error("Restart file contained errors.\n");
				ProcessPool::finalise();
				return 1;
			}
		}
		else Messenger::print("\nRestart file '%s' does not exist.\n", restartFile.get());
	}
	else Messenger::print("\nRestart file (if it exists) will be ignored.\n");

	refreshing_ = true;

	// Add on necessary tabs
	addCoreTabs();
	ListIterator<Configuration> configIterator(duq_.configurations());
	while (Configuration* cfg = configIterator.iterate()) addConfigurationTab(cfg);
	addProcessingTab();
	refreshing_ = false;

	// Does a window state exist for this input file?
	windowLayoutFilename_.sprintf("%s.state", duq_.inputFilename());

	// Try to load in the window state file
	if (DUQSys::fileExists(windowLayoutFilename_) && (!ignoreLayoutFile)) loadWindowLayout();

	updateControls();

	updateStatus();

	return true;
}

/*
 * Update Functions
 */

// Refresh all displayed widgets
void DUQWindow::updateControls()
{
	// Iteration Panel
	ui.IterationNumberLabel->setText(DUQSys::itoa(duq_.iteration()));

	// Loop over tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next) tab->updateControls();
}

// Update status
void DUQWindow::updateStatus()
{
	// Window Title
	QString title = QString("%1%2").arg(duq_.hasInputFileName() ? duq_.inputFilename() : "<untitled>", modified_ ? "(*)" : ""); 
	setWindowTitle(title);

	// dUQ Status
	QPalette labelPalette = ui.SetUpLabel->palette();
	if (duq_.isSetUp())
	{
		labelPalette.setColor(QPalette::WindowText, Qt::darkGreen);
		ui.SetUpLabel->setText("Set Up & Ready");
	}
	else
	{
		labelPalette.setColor(QPalette::WindowText, Qt::darkRed);
		ui.SetUpLabel->setText("Not Set Up");
	}
	ui.SetUpLabel->setPalette(labelPalette);
}

// Link output handler in to the Messenger
void DUQWindow::addOutputHandler()
{
	Messenger::setOutputHandler(&outputHandler_);
	connect(&outputHandler_, SIGNAL(printText(const QString&)), ui.MessagesBrowser, SLOT(append(const QString&)));
}

/*
 * SubWidgets
 */

// Create named SubWidget
SubWidget* DUQWindow::createSubWidget(const char* widgetName, const char* title)
{
	SubWidget* subWidget = NULL;

	if (DUQSys::sameString(widgetName, "PairPotential"))
	{
		PairPotentialWidget* ppWidget = new PairPotentialWidget(this, title);
		subWidget = ppWidget;
	}
	else if (DUQSys::sameString(widgetName, "ModuleControl"))
	{
		ModuleControlWidget* moduleWidget = new ModuleControlWidget(this, NULL, title);
		connect(moduleWidget, SIGNAL(moduleRun()), this, SLOT(updateControls()));
		subWidget = moduleWidget;
	}
	else Messenger::error("Don't know how to create SubWidget of type '%s'.\n", widgetName);

	return subWidget;
}

/*
 * Window State
 */

// Save current window layout
bool DUQWindow::saveWindowLayout()
{
	// Open file for writing
	LineParser stateParser;
	stateParser.openOutput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForWriting()) return false;

	// Main GUI elemnets
	// -- Current tab
	if (!stateParser.writeLineF("%i\n", ui.MainTabs->currentIndex())) return false;

	// Loop over tabs
	for (MainTab* tab = tabs_.first(); tab != NULL; tab = tab->next) if (!tab->writeState(stateParser)) return false;

	stateParser.closeFiles();

	return true;
}

// Load window layout
bool DUQWindow::loadWindowLayout()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	// Main GUI elemnets
	// -- Current tab - it may not yet exist, so store it now and set it later
	if (stateParser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;
	int currentTab = stateParser.argi(0);

	// Remainder of file references widgets / modules in some order
	while (!stateParser.eofOrBlank())
	{
		// Parse the line
		if (stateParser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;

		SubWidget* subWidget = NULL;
		SubWindow* subWindow = NULL;

		// The line should contain the title of the target mdiArea, the type of the widget we should create in a subwindow, and the subwindow title
		MainTab* targetTab = findTab(stateParser.argc(0));
		if (!targetTab)
		{
			Messenger::printVerbose("Tab titled '%s' does not yet exist, so we will create it now...\n");
			targetTab = addWorkspaceTab(stateParser.argc(0));
		}

		// We now check the availability of an area for SubWindows in the tab.
		// If there is one then we must create the window and add it to the tab before reading its state.
		// If not, we search for the named sub *widget*, which should already exist in the tab.
		if (targetTab->subWindowArea())
		{
			subWidget = createSubWidget(stateParser.argc(1), stateParser.argc(2));

			if (subWidget) subWindow = targetTab->addSubWindow(subWidget, NULL);
			else
			{
				Messenger::error("Couldn't read state information - unrecognised widget type '%s' encountered.\n", stateParser.argc(1));
				return false;
			}
		}
		else
		{
			// No SubWindow area, so just try to find the named widget (which should already exist)
			subWidget = targetTab->findSubWidget(stateParser.argc(2));
			if (!subWidget)
			{
				Messenger::error("Couldn't read state information - widget '%s' not found in tab '%s'.\n", stateParser.argc(2), stateParser.argc(0));
				return false;
			}
		}

		// Read in the widget's geometry / state / flags (depending on whether it went into a new SubWindow or is just a SubWidget)
		if (targetTab->subWindowArea())
		{
			if (stateParser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			QMdiSubWindow* window = subWindow->window();
			window->setGeometry(stateParser.argi(0), stateParser.argi(1), stateParser.argi(2), stateParser.argi(3));
			// -- Is the window maximised, or shaded?
			if (stateParser.argb(4)) window->showMaximized();
			else if (stateParser.argb(5)) window->showShaded();
		}
		else
		{
			// Discard line, which is not currently used (left in for future use)
			if (stateParser.skipLines(1) != LineParser::Success) return false;
		}

		// Now call the widget's local readState()
		if (!subWidget->readState(stateParser)) return false;
	}

	// Set current tab (we store the index earlier)
	ui.MainTabs->setCurrentIndex(currentTab);

	return true;
}

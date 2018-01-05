/*
	*** Monitor Window - Functions
	*** src/gui/monitor_funcs.cpp
	Copyright T. Youngs 2007-2018

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
#include "gui/browser.h"
#include "gui/modulecontrolwidget.h"
#include "gui/pairpotentialwidget.h"
#include "gui/subwidget.h"
#include "gui/thread.hui"
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

	// Set window state filename
	windowStateFilename_.sprintf("%s.mon", duq_.filename());

	// Try to load in the window state file
	if (!loadWindowState())
	{
		// Create a new BrowserWidget
		BrowserWidget* browserWidget = new BrowserWidget(NULL, *this, duq_);
		addWindow(browserWidget, &duq_, "Browser");
// 		// Add default subwindows
// 		ui.MainArea->addSubWindow(new BrowserWindow(*this, duq_));
	}

	// Link our output handler to the Messenger, and connect up signals/slots
	Messenger::setOutputHandler(&outputHandler_);
	connect(&outputHandler_, SIGNAL(printText(const QString&)), ui.MessagesBrowser, SLOT(append(const QString&)));

	// Connect signals to thread controller
	connect(this, SIGNAL(iterate(int)), &threadController_, SLOT(iterate(int)));
	connect(this, SIGNAL(stopIterating()), &threadController_, SLOT(stopIterating()));

	refreshing_ = false;
}

// Destructor
DUQWindow::~DUQWindow()
{
}

// Return reference to dUQ
DUQ& DUQWindow::duq()
{
	return duq_;
}

// Catch window close event
void DUQWindow::closeEvent(QCloseEvent* event)
{
	// Save the state before we go...
	saveWindowState();

	event->accept();
}

void DUQWindow::resizeEvent(QResizeEvent* event)
{
}

/*
 * Update Functions
 */

// Refresh all displayed widgets
void DUQWindow::updateWidgets()
{
	// Iteration Panel
	ui.IterationNumberLabel->setText(DUQSys::itoa(duq_.iteration()));
	ui.IterationLimitLabel->setText(duq_.maxIterations() == -1 ?  QString(QChar(0x221E)) : QString::number(duq_.maxIterations()));

	// Sub-windows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->updateControls();
}

// Set widgets ready for the main code to be run
void DUQWindow::setWidgetsForRun()
{
	// Disable run and step buttons, and enable pause button
	ui.ControlRunButton->setEnabled(false);
	ui.ControlStepButton->setEnabled(false);
	ui.ControlStepFiveButton->setEnabled(false);
	ui.ControlPauseButton->setEnabled(true);

	// Disable necessary controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->disableSensitiveControls();
}

// Set widgets after the main code has been run
void DUQWindow::setWidgetsAfterRun()
{
	// Enable run and step buttons, and disable pause button
	ui.ControlRunButton->setEnabled(true);
	ui.ControlStepButton->setEnabled(true);
	ui.ControlStepFiveButton->setEnabled(true);
	ui.ControlPauseButton->setEnabled(false);

	// Enable necessary controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->enableSensitiveControls();
}

// All iterations requested are complete
void DUQWindow::iterationsComplete()
{
	setWidgetsAfterRun();
}

/*
 * Sub-window Management
 */

// Find SubWindow from specified data pointer
SubWindow* DUQWindow::subWindow(void* data)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == data) return subWindow;

	return NULL;
}

// Return window for specified data (as pointer), if it exists
QMdiSubWindow* DUQWindow::currentWindow(void* windowContents)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowContents) return subWindow->window();

	return NULL;
}

// Add window for specified data (as pointer)
QMdiSubWindow* DUQWindow::addWindow(SubWidget* widget, void* windowContents, const char* windowTitle)
{
	// Check that the windowContents aren't currently in the list
	QMdiSubWindow* window = windowContents ? currentWindow(windowContents) : NULL;
	if (window)
	{
		Messenger::print("Refused to add window contents %p to our list, as it is already present elsewhere.\n");
		return window;
	}

	// Create a new QMdiSubWindow, show, and update controls
	window = ui.MainArea->addSubWindow(widget);
	connect(widget, SIGNAL(windowClosed(void*)), this, SLOT(removeWindow(void*)));
	window->setWindowTitle(windowTitle);
	window->show();
	widget->updateControls();

	// Store window / widget data in our list
	SubWindow* subWindow = new SubWindow(window, widget, windowContents);
	subWindows_.own(subWindow);

	return window;
}

// Remove window for specified data (as pointer), removing it from the list
bool DUQWindow::removeWindow(void* windowContents)
{
	// Find the windowContents the list
	SubWindow* window = subWindow(windowContents);
	if (!window)
	{
		Messenger::print("Couldn't remove window containing contents %p from list, since it is not present.\n", windowContents);
		return false;
	}

	subWindows_.remove(window);

	return true;
}

/*
 * Window State
 */

// Save current window state
bool DUQWindow::saveWindowState()
{
	// Open file for writing
	LineParser stateParser;
	stateParser.openOutput(windowStateFilename_);
	if (!stateParser.isFileGoodForWriting()) return false;

	// Loop over our subwindow list
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate())
	{
		// Write window geometry / state
		if (!stateParser.writeLineF("%s '%s'\n", subWindow->subWidget()->widgetType(), qPrintable(subWindow->window()->windowTitle()))) return false;
		QRect geometry = subWindow->window()->geometry();
		if (!stateParser.writeLineF("%i %i %i %i %s %s\n", geometry.x(), geometry.y(), geometry.width(), geometry.height(), DUQSys::btoa(subWindow->window()->isMaximized()), DUQSys::btoa(subWindow->window()->isShaded()))) return false;
		if (!subWindow->subWidget()->writeState(stateParser)) return false;
	}

	stateParser.closeFiles();

	return true;
}

// Load window state
bool DUQWindow::loadWindowState()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(windowStateFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	while (!stateParser.eofOrBlank())
	{
		// Parse the line
		if (stateParser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;

		SubWidget* subWidget = NULL;
		QMdiSubWindow* subWindow = NULL;

		// The line should contain the name of the widget we should create in a subwindow, and the subwindow title
		if (DUQSys::sameString(stateParser.argc(0), "Browser"))
		{
			BrowserWidget* browserWidget = new BrowserWidget(NULL, *this, duq_);
			subWindow = addWindow(browserWidget, &duq_, stateParser.argc(1));
			subWidget = browserWidget;
		}
		else if (DUQSys::sameString(stateParser.argc(0), "PairPotential"))
		{
			PairPotentialWidget* ppWidget = new PairPotentialWidget(NULL, NULL, duq_);
			subWindow = addWindow(ppWidget, NULL, stateParser.argc(1));
			subWidget = ppWidget;
		}
		else if (DUQSys::sameString(stateParser.argc(0), "ModuleControl"))
		{
			ModuleControlWidget* moduleWidget = new ModuleControlWidget(NULL, NULL, duq_);
			connect(moduleWidget, SIGNAL(moduleRun()), this, SLOT(updateWidgets()));
			subWindow = addWindow(moduleWidget, NULL, stateParser.argc(1));
			subWidget = moduleWidget;
		}

		// Did we recognise the widget?
		if (!subWidget)
		{
			Messenger::error("Couldn't read state information - unrecognised widget type '%s' encountered.\n", stateParser.argc(0));
			return false;
		}

		// Read in the widget's geometry / state
		if (stateParser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		subWindow->setGeometry(stateParser.argi(0), stateParser.argi(1), stateParser.argi(2), stateParser.argi(3));
		// -- Is the window maximised, or shaded?
		if (stateParser.argb(4)) subWindow->showMaximized();
		else if (stateParser.argb(5)) subWindow->showShaded();

		// Now call the widget's local readState()
		if (!subWidget->readState(stateParser)) return false;
	}

	return true;
}

/*
 * Widget Slots
 */

void DUQWindow::on_ControlRunButton_clicked(bool checked)
{
	// Prepare the GUI
	setWidgetsForRun();

	emit iterate(-1);
}

void DUQWindow::on_ControlStepButton_clicked(bool checked)
{
	// Prepare the GUI
	setWidgetsForRun();

	emit iterate(1);
}

void DUQWindow::on_ControlStepFiveButton_clicked(bool checked)
{
	// Prepare the GUI
	setWidgetsForRun();

	emit iterate(5);
}

void DUQWindow::on_ControlPauseButton_clicked(bool checked)
{
	emit stopIterating();
}

void DUQWindow::on_ControlReloadButton_clicked(bool checked)
{
}

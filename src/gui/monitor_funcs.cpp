/*
	*** Monitor Window - Functions
	*** src/gui/monitor_funcs.cpp
	Copyright T. Youngs 2007-2017

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
#include "gui/monitor.h"
#include "gui/browser.h"
#include "gui/modulecontrolwidget.h"
#include "gui/pairpotentialwidget.h"
#include "gui/subwidget.h"
#include "base/lineparser.h"
#include <QCloseEvent>
#include <QMdiSubWindow>

// Static Members
QTextBrowser* MonitorWindow::messagesBrowser_ = NULL;

// Constructor
MonitorWindow::MonitorWindow(DUQ& duq) : QMainWindow(NULL), duq_(duq)
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

	// Store pointer to QTextBrowser used for messaging
	messagesBrowser_ = ui.MessagesBrowser;

	refreshing_ = false;
}

// Destructor
MonitorWindow::~MonitorWindow()
{
}

// Return reference to dUQ
DUQ& MonitorWindow::duq()
{
	return duq_;
}

// Catch window close event
void MonitorWindow::closeEvent(QCloseEvent* event)
{
	// Save the state before we go...
	saveWindowState();

	event->accept();
}

void MonitorWindow::resizeEvent(QResizeEvent* event)
{
}

/*
 * Update Functions
 */

// Return QTextBrowser for GUI messaging
QTextBrowser* MonitorWindow::messagesBrowser()
{
	return messagesBrowser_;
}


// Refresh specified aspects of the window
void MonitorWindow::updateWidgets()
{
	// Iteration Panel
	ui.IterationNumberLabel->setText(DUQSys::itoa(duq_.iteration()));
	ui.IterationLimitLabel->setText(duq_.maxIterations() == -1 ?  QString(QChar(0x221E)) : QString::number(duq_.maxIterations()));

	// Sub-windows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->updateControls();
}

/*
 * Sub-window Management
 */

// Find SubWindow from specified data pointer
SubWindow* MonitorWindow::subWindow(void* data)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == data) return subWindow;

	return NULL;
}

// Return window for specified data (as pointer), if it exists
QMdiSubWindow* MonitorWindow::currentWindow(void* windowContents)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowContents) return subWindow->window();

	return NULL;
}

// Add window for specified data (as pointer)
QMdiSubWindow* MonitorWindow::addWindow(SubWidget* widget, void* windowContents, const char* windowTitle)
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
bool MonitorWindow::removeWindow(void* windowContents)
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
bool MonitorWindow::saveWindowState()
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
bool MonitorWindow::loadWindowState()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(windowStateFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	while (!stateParser.eofOrBlank())
	{
		// Parse the line
		if (stateParser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		SubWidget* subWidget = NULL;
		QMdiSubWindow* subWindow = NULL;

		// The only argument on the line should be the name of the widget we should create in a subwindow
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

void MonitorWindow::on_IterateButton_clicked(bool checked)
{
	duq_.iterate(1);

	updateWidgets();
}

void MonitorWindow::on_IterateFiveButton_clicked(bool checked)
{
	duq_.iterate(5);

	updateWidgets();
}

/*
	*** Main Tab
	*** src/gui/maintab.cpp
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

#include "gui/maintab.h"
#include "gui/subwindow.h"
#include "gui/subwidget.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QTabWidget>
#include <QMdiArea>
#include <QMdiSubWindow>

// Constructor / Destructor
MainTab::MainTab(DUQ& duq, QTabWidget* tabWidget, const char* name) : ListItem<MainTab>(), duq_(duq)
{
	tabWidget_ = tabWidget;
	name_ = name;
}

MainTab::~MainTab()
{
	// Delete the tab from the tabwidget first - find its index (based on our, the page's, pointer) and remove that
	int index = tabWidget_->indexOf( (QWidget*) this);
	if (index == -1) Messenger::print("Couldn't remove tab '%s' since its page widget could not be found.\n", name_.get());
	else
	{
		tabWidget_->removeTab(index);
	}
}


/*
 * Data
 */

// Return name of tab
const char* MainTab::name() const
{
	return name_.get();
}

/*
 * MDI SubWindow Management
 */

// Find SubWindow from specified data pointer
SubWindow* MainTab::subWindow(void* data)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == data) return subWindow;

	return NULL;
}

// Add window for specified data (as pointer)
QMdiSubWindow* MainTab::addWindowToMDIArea(SubWidget* widget, void* windowContents, const char* windowTitle)
{
	// If we have no MDI area, exit now!
	if (!mdiArea())
	{
		Messenger::error("No MDI area exists in tab '%s' to place a subwindow in.\n", name_.get());
		return NULL;
	}

	// Check that the windowContents aren't currently in the list
	QMdiSubWindow* window = windowContents ? findMDIWindow(windowContents) : NULL;
	if (window)
	{
		Messenger::printVerbose("Refused to add window contents %p to our list, as it is already present elsewhere. It will be raised instead.\n");
		window->raise();
		return window;
	}

	// Create a new QMdiSubWindow, show, and update controls
	window = mdiArea()->addSubWindow(widget);
	window->setWindowTitle(windowTitle);
	window->show();
	widget->updateControls();

	// Store window / widget data in our list
	SubWindow* subWindow = new SubWindow(window, widget, windowContents);
	subWindows_.own(subWindow);

	return window;
}

// Return window for specified data (as pointer), if it exists
QMdiSubWindow* MainTab::findMDIWindow(void* windowContents)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowContents) return subWindow->window();

	return NULL;
}

// Return window with specified title, if it exists
QMdiSubWindow* MainTab::findMDIWindow(const char* title)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->window()->windowTitle() == title) return subWindow->window();

	return NULL;
}

// Remove window for specified data (as pointer), removing it from the list
bool MainTab::removeWindowFromMDIArea(void* windowContents)
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
 * Update
 */

// Update all controls in the tab
void MainTab::updateControls()
{
	// Update any controls specific to the tab
	updatePage();

	// Update any MDI subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->updateControls();
}

// Disable sensitive controls within tab, ready for main code to run
void MainTab::disableSensitiveControls()
{
	// Disable sensitive controls in page
	disableSensitiveControlsInPage();

	// Disable sensitive controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void MainTab::enableSensitiveControls()
{
	// Enable sensitive controls in page
	enableSensitiveControlsInPage();

	// Enable sensitive controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->enableSensitiveControls();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool MainTab::writeState(LineParser& parser)
{
	// Loop over our subwindow list
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate())
	{
		// Write window geometry / state
		if (!parser.writeLineF("%s  %s '%s'\n", name_.get(), subWindow->subWidget()->widgetType(), qPrintable(subWindow->window()->windowTitle()))) return false;
		QRect geometry = subWindow->window()->geometry();
		if (!parser.writeLineF("%i %i %i %i %s %s\n", geometry.x(), geometry.y(), geometry.width(), geometry.height(), DUQSys::btoa(subWindow->window()->isMaximized()), DUQSys::btoa(subWindow->window()->isShaded()))) return false;
		if (!subWindow->subWidget()->writeState(parser)) return false;
	}

	return true;
}

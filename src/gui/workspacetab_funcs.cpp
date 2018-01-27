/*
	*** WorkspaceTab Functions
	*** src/gui/workspacetab_funcs.cpp
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

#include "gui/workspacetab.h"
#include "gui/subwidget.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <QMdiArea>
#include <QMdiSubWindow>

// Constructor / Destructor
WorkspaceTab::WorkspaceTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);
}

WorkspaceTab::~WorkspaceTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return the tab's SubWindow area, if it has one
QMdiArea* WorkspaceTab::subWindowArea()
{
	return ui.WorkspaceArea;
}

// Return the tab's SubWidget layout, if it has one
QLayout* WorkspaceTab::subWidgetLayout()
{
	return NULL;
}

/*
 * Update
 */

// Update controls in tab
void WorkspaceTab::updateControls()
{
	// Update our MDI subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->updateControls();
}

// Disable sensitive controls within tab, ready for main code to run
void WorkspaceTab::disableSensitiveControls()
{
	// Disable sensitive controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void WorkspaceTab::enableSensitiveControls()
{
	// Enable sensitive controls in subwindows
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) subWindow->subWidget()->enableSensitiveControls();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool WorkspaceTab::writeState(LineParser& parser)
{
	// Loop over our subwindow list
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate())
	{
		// Write window geometry / state
		if (!parser.writeLineF("'%s'  %s  '%s'\n", title_.get(), subWindow->subWidget()->widgetType(), subWindow->subWidget()->title())) return false;
		QRect geometry = subWindow->window()->geometry();
		if (!parser.writeLineF("%i %i %i %i %s %s\n", geometry.x(), geometry.y(), geometry.width(), geometry.height(), DUQSys::btoa(subWindow->window()->isMaximized()), DUQSys::btoa(subWindow->window()->isShaded()))) return false;
		if (!subWindow->subWidget()->writeState(parser)) return false;
	}

	return true;
}

/*
 * MDI SubWindow Management
 */

// Find SubWindow from specified data pointer
SubWindow* WorkspaceTab::subWindow(void* data)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == data) return subWindow;

	return NULL;
}

// Return window for specified data (as pointer), if it exists
QMdiSubWindow* WorkspaceTab::findMDIWindow(void* windowContents)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowContents) return subWindow->window();

	return NULL;
}

// Remove window for specified data (as pointer), removing it from the list
bool WorkspaceTab::removeWindowFromMDIArea(void* windowContents)
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

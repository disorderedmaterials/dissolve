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
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/widgets/subwindow.h"
#include "gui/widgets/subwidget.h"
#include "module/module.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QFrame>
#include <QLayout>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTabWidget>

// Constructor / Destructor
MainTab::MainTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* tabWidget, const char* title, QWidget* page) : ListItem<MainTab>(), duq_(duq)
{
	duqWindow_ = duqWindow;
	tabWidget_ = tabWidget;
	page_ = page;
	title_ = title;

	// Add tab to the parent tabwidget
	if (tabWidget_) tabWidget_->addTab(page_, title_.get());
}

MainTab::~MainTab()
{
	// Delete the tab from the tabwidget first - find its index (based on our, the page's, pointer) and remove that
	int index = tabWidget_->indexOf(page_);
	if (index == -1) printf("Couldn't remove tab '%s' since its page widget could not be found.\n", title_.get());
	else
	{
		tabWidget_->removeTab(index);
	}
}

/*
 * Data
 */

// Set title of tab
void MainTab::setTitle(const char* title)
{
	if (canChangeTitle())
	{
		// Find ourselves in the tab widget
		int index = tabWidget_->indexOf(page_);
		if (index == -1)
		{
			Messenger::print("Couldn't rename tab '%s' since its page widget could not be found.\n", title_.get());
			return;
		}
		title_ = title;
		tabWidget_->setTabText(index, title);
	}
	else Messenger::warn("Refusing to change title of tab currently named '%s', since it is fixed.\n", title_.get());
}

// Return title of tab
const char* MainTab::title() const
{
	return title_.get();
}

// Return whether the title of the tab can be changed
bool MainTab::canChangeTitle()
{
	return false;
}

/*
 * SubWidget / SubWindow Handling
 */

// Find SubWindow by title
SubWindow* MainTab::findSubWindow(const char* title)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (DUQSys::sameString(subWindow->subWidget()->title(), title)) return subWindow;

	return NULL;
}

// Find SubWindow by SubWidget
SubWindow* MainTab::findSubWindow(SubWidget* subWidget)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == subWidget) return subWindow;

	return NULL;
}

// Find SubWindow by data content
SubWindow* MainTab::findSubWindow(void* windowData)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowData) return subWindow;

	return NULL;
}

// Find and return named SubWidget
SubWidget* MainTab::findSubWidget(const char* widgetTitle)
{
	ListIterator<SubWidget> widgetIterator(subWidgets_);
	while (SubWidget* subWidget = widgetIterator.iterate()) if (DUQSys::sameString(widgetTitle, subWidget->title())) return subWidget;
	return NULL;
}

// Add SubWindow for widget containing specified data (as pointer)
SubWindow* MainTab::addSubWindow(SubWidget* widget, void* windowContents)
{
	// Make sure that the tab has a SubWindow area
	QMdiArea* mdiArea = subWindowArea();
	if (!mdiArea)
	{
		Messenger::error("This tab doesn't have a SubWidget area, so can't add a new SubWindow.\n");
		return NULL;
	}

	// Check that the windowContents aren't currently in the list
	SubWindow* subWindow = windowContents ? findSubWindow(windowContents) : NULL;
	if (subWindow)
	{
		Messenger::printVerbose("Refused to add window contents %p to our list, as it is already present elsewhere. It will be raised instead.\n");
		subWindow->window()->raise();
		return subWindow;
	}

	// Create a new QMdiSubWindow, show, and update controls
	QMdiSubWindow* newMDIWindow = subWindowArea()->addSubWindow(widget);
	newMDIWindow->setWindowTitle(widget->title());
	newMDIWindow->show();
	widget->updateControls();

	// Store window / widget data in our list
	subWindow = new SubWindow(newMDIWindow, widget, windowContents);
	subWindows_.own(subWindow);

	return subWindow ;
}

// Remove SubWindow specifiedf
void MainTab::removeSubWindow(SubWindow* window)
{
	subWindows_.remove(window);
}

/*
 * Helper Functions
 */

// Add module widgets to specified layout
void MainTab::addModuleWidgets(const RefList<Module,bool>& modules, List<SubWidget>& widgets, QLayout* layout)
{
	RefListIterator<Module,bool> moduleIterator(modules);
	while (Module* module = moduleIterator.iterate())
	{
		if (!moduleIterator.first())
		{
			QFrame* frame = new QFrame;
			frame->setFrameShape(QFrame::VLine);
			layout->addWidget(frame);
		}

		ModuleControlWidget* moduleWidget = new ModuleControlWidget(duqWindow_, module, CharString("%s (%s)", module->name(), module->uniqueName()));
		QObject::connect(moduleWidget, SIGNAL(moduleRun()), duqWindow_, SLOT(updateControls()));
		layout->addWidget(moduleWidget);
		widgets.own(moduleWidget);
	}
}

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
#include "gui/gui.h"
#include "gui/tmdiarea.hui"
#include "gui/modulecontrolwidget.h"
#include "gui/pairpotentialwidget.h"
#include "gui/widgets/subwidget.h"
#include "main/duq.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/variantpointer.h"
#include <QMdiSubWindow>
#include <QMenu>

// Constructor / Destructor
WorkspaceTab::WorkspaceTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);

	// Add a TMdiArea to the main layout
	mdiArea_ = new TMdiArea(duqWindow);
	ui.verticalLayout->addWidget(mdiArea_);
	connect(mdiArea_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

WorkspaceTab::~WorkspaceTab()
{
}

/*
 * Data
 */

// Return whether the title of the tab can be changed
bool WorkspaceTab::canChangeTitle()
{
	return true;
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

// Remove subWindow with specified title
void WorkspaceTab::removeSubWindow(QString title)
{
	// Find the named subwindow
	SubWindow* window = findSubWindow(qPrintable(title));
	if (window) removeSubWindow(window);
}

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

// Find SubWindow by title
SubWindow* WorkspaceTab::findSubWindow(const char* title)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (DUQSys::sameString(subWindow->subWidget()->title(), title)) return subWindow;

	return NULL;
}

// Find SubWindow by SubWidget
SubWindow* WorkspaceTab::findSubWindow(SubWidget* subWidget)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == subWidget) return subWindow;

	return NULL;
}

// Find SubWindow by data content
SubWindow* WorkspaceTab::findSubWindow(void* windowData)
{
	ListIterator<SubWindow> subWindowIterator(subWindows_);
	while (SubWindow* subWindow = subWindowIterator.iterate()) if (subWindow->data() == windowData) return subWindow;

	return NULL;
}

// Add SubWindow for widget containing specified data (as pointer)
SubWindow* WorkspaceTab::addSubWindow(SubWidget* widget, void* windowContents)
{
	// Check that the windowContents aren't currently in the list
	SubWindow* subWindow = windowContents ? findSubWindow(windowContents) : NULL;
	if (subWindow)
	{
		Messenger::printVerbose("Refused to add window contents %p to our list, as it is already present elsewhere. It will be raised instead.\n");
		subWindow->window()->raise();
		return subWindow;
	}

	// Create a new QMdiSubWindow, show, and update controls
	QMdiSubWindow* newMDIWindow = mdiArea_->addSubWindow(widget);
	newMDIWindow->setWindowTitle(widget->title());
	newMDIWindow->show();
	widget->updateControls();

	// Store window / widget data in our list
	subWindow = new SubWindow(newMDIWindow, widget, windowContents);
	subWindows_.own(subWindow);

	return subWindow ;
}

// Remove SubWindow specified
void WorkspaceTab::removeSubWindow(SubWindow* window)
{
	subWindows_.remove(window);
}

/*
 * Context Menu
 */

// Create Module menu with specified QMenu as parent
void WorkspaceTab::createContextMenu(QMenu* parent)
{
	QFont italicFont = font();
	italicFont.setItalic(true);

	// General widgets, not associated to a module
	QAction* menuItem = parent->addAction("General");
	menuItem->setFont(italicFont);
	menuItem->setEnabled(false);
	menuItem = parent->addAction("PairPotential");
	connect(menuItem, SIGNAL(triggered(bool)), this, SLOT(contextMenuWidgetSelected(bool)));

	// Modules within Configurations
	menuItem = parent->addAction("Configurations");
	menuItem->setFont(italicFont);
	menuItem->setEnabled(false);
	ListIterator<Configuration> configIterator(duq_.configurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		QMenu* cfgMenu = parent->addMenu(cfg->name());
		if (cfg->nModules() == 0)
		{
			QAction* moduleItem = cfgMenu->addAction("No Local Modules");
			moduleItem->setFont(italicFont);
			moduleItem->setEnabled(false);
		}
		ListIterator<ModuleReference> moduleIterator(cfg->modules().modules());
		while (ModuleReference* modRef = moduleIterator.iterate())
		{
			Module* module = modRef->module();

			QAction* moduleItem = cfgMenu->addAction(CharString("%s (%s)", module->name(), module->uniqueName()).get());
			moduleItem->setData(VariantPointer<Module>(module));
			connect(moduleItem, SIGNAL(triggered(bool)), this, SLOT(contextMenuModuleSelected(bool)));
		}
	}

	// Processing Modules
	menuItem = parent->addAction("Processing");
	menuItem->setFont(italicFont);
	menuItem->setEnabled(false);
	if (duq_.processingModules().nModules() == 0)
	{
		QAction* moduleItem = parent->addAction("None");
		moduleItem->setFont(italicFont);
		moduleItem->setEnabled(false);
	}
	ListIterator<ModuleReference> moduleIterator(duq_.processingModules().modules());
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* module = modRef->module();

		QAction* moduleItem = parent->addAction(CharString("%s (%s)", module->name(), module->uniqueName()).get());
		moduleItem->setData(VariantPointer<Module>(module));
		connect(moduleItem, SIGNAL(triggered(bool)), this, SLOT(contextMenuModuleSelected(bool)));
	}
}

// Custom context menu requested
void WorkspaceTab::showContextMenu(const QPoint& pos)
{
	QMenu menu;
	menu.setFont(font());
	QMenu* subMenu = menu.addMenu("Add &Widget");
	createContextMenu(subMenu);
	menu.exec(mapToGlobal(pos));
}

// Module selected on context menu
void WorkspaceTab::contextMenuModuleSelected(bool checked)
{
	// Get the sender QAction
	QAction* action = (QAction*) sender();
	if (!action) return;

	// If the QAction's data is valid then it should contain a Module pointer
	Module* module = VariantPointer<Module>(action->data());
	if (!module) return;

	addModuleControlWidget(module);
}

// General widget selected on context menu
void WorkspaceTab::contextMenuWidgetSelected(bool checked)
{
	// Get the sender QAction
	QAction* action = (QAction*) sender();
	if (!action) return;

	// The text of the sender QAction is the name of the widget we need to create
	addNamedWidget(qPrintable(action->text()), qPrintable(action->text()));
}

// Add ModuleControl widget to workspace
void WorkspaceTab::addModuleControlWidget(Module* module)
{
	// Is the Module already displayed?
	SubWindow* window = findSubWindow(CharString("%s (%s)", module->name(), module->uniqueName()));
	if (!window)
	{
		// Create a new ModuleWidget
		ModuleControlWidget* moduleControlWidget = new ModuleControlWidget(duqWindow_, module, CharString("%s (%s)", module->name(), module->uniqueName()), false);
		connect(moduleControlWidget, SIGNAL(windowClosed(QString)), this, SLOT(removeSubWindow(QString)));
		window = addSubWindow(moduleControlWidget, module);

		// If we are currently running, we need to disable sensitive controls in the newly-created widget
		if ((duqWindow_->duqState() != DUQWindow::StoppedState) && window && window->subWidget()) window->subWidget()->disableSensitiveControls();
	}
	else window->raise();
}

// Add named widget to workspace
void WorkspaceTab::addNamedWidget(const char* widgetName, const char* title)
{
	// Make sure we have a unique title for the widget
	CharString uniqueTitle = title;
	int index = 0;
	while (findSubWindow(uniqueTitle)) uniqueTitle.sprintf("%s%02i", title, ++index);

	SubWidget* subWidget = NULL;
        if (DUQSys::sameString(widgetName, "PairPotential"))
        {
                PairPotentialWidget* ppWidget = new PairPotentialWidget(duqWindow_, uniqueTitle);
                connect(ppWidget, SIGNAL(windowClosed(QString)), this, SLOT(removeSubWindow(QString)));
                subWidget = ppWidget;
        }
	else
	{
		Messenger::error("Couldn't add widget to current workspace - unrecognised widget type '%s' encountered.\n", widgetName);
		return;
	}

	if (subWidget) addSubWindow(subWidget, NULL);
}

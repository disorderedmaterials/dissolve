/*
	*** dUQ Main Window - Menu Functions
	*** src/gui/gui_menu.cpp
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

#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "main/duq.h"
#include "templates/variantpointer.h"
#include <QInputDialog>

/*
 * Main Menu
 */

void DUQWindow::on_SessionOpenAction_triggered(bool checked)
{
}

void DUQWindow::on_SessionSaveAction_triggered(bool checked)
{
	// Attempt to save the file (TEST)
	duq_.saveInput("shitty.txt");

	modified_ = false;

	updateStatus();
}

void DUQWindow::on_SessionQuitAction_triggered(bool checked)
{
}

void DUQWindow::addWidgetToCurrentWorkspace(bool checked)
{
	// Get the sender QAction
	QAction* action = (QAction*) sender();
	if (!action) return;

	// Get current tab and check it has an MDI area
	MainTab* tab = currentTab();
	if (!tab) return;
	if (!tab->subWindowArea())
	{
		Messenger::error("No SubWindow (MDI) workspace, so can't add a widget to it.\n");
		return;
	}

	// If the QAction's data is valid then it should indicate a specific Module.
	// Otherwise we add a new widget by the name of the QAction.
	SubWindow* window = NULL;
	Module* module = VariantPointer<Module>(action->data());
	if (module)
	{
		// Is the Module already displayed?
		window = tab->findSubWindow(CharString("%s (%s)", module->name(), module->uniqueName()));
		if (!window)
		{
			// Create a new ModuleWidget
			ModuleControlWidget* moduleControlWidget = new ModuleControlWidget(this, module, CharString("%s (%s)", module->name(), module->uniqueName()), false);
			connect(moduleControlWidget, SIGNAL(moduleRun()), this, SLOT(updateControls()));
			connect(moduleControlWidget, SIGNAL(windowClosed(QString)), this, SLOT(removeWidgetFromCurrentWorkspace(QString)));
			window = tab->addSubWindow(moduleControlWidget, module);
		}
		else window->raise();
	}
	else
	{
		// Make sure we have a unique title for the widget
		CharString title = qPrintable(action->text());
		int index = 0;
		while (tab->findSubWindow(title)) title.sprintf("%s%02i", qPrintable(action->text()), ++index);

		SubWidget* subWidget = createSubWidget(qPrintable(action->text()), title);

		if (subWidget) window = tab->addSubWindow(subWidget, NULL);
		else
		{
			Messenger::error("Couldn't add widget to current workspace - unrecognised widget type '%s' encountered.\n", qPrintable(action->text()));
		}
	}

	// If we are currently running, we need to disable sensitive controls in the newly-created widget
	if ((duqState_ != DUQWindow::StoppedState) && window && window->subWidget()) window->subWidget()->disableSensitiveControls();
}

void DUQWindow::removeWidgetFromCurrentWorkspace(QString windowTitle)
{
	// Get current tab and check it has an MDI area
	MainTab* tab = currentTab();
	if (!tab) return;
	if (!tab->subWindowArea())
	{
		Messenger::error("No SubWindow (MDI) workspace, so can't remove a widget from it.\n");
		return;
	}

	// First, assume the passed windowData is the actual window data displayed.
	SubWindow* window = tab->findSubWindow(qPrintable(windowTitle));
	if (window) tab->removeSubWindow(window);
}

void DUQWindow::on_WorkspaceAddNewAction_triggered(bool checked)
{
	// Add a new workspace
	bool ok;
	QString text = QInputDialog::getText(this, "New Workspace", "Enter the name of the new workspace", QLineEdit::Normal, "New Workspace", &ok);
	if (!ok || text.isEmpty()) return;

	MainTab* workspaceTab = addWorkspaceTab(qPrintable(text));

	setCurrentTab(workspaceTab);
}

// Update menu items (after change in Modules etc.)
void DUQWindow::updateMenuItems()
{
	// Update the WorkSpaceAddWidget submenu
	ui.WorkspaceAddWidgetAction->clear();

	createModuleMenu(ui.WorkspaceAddWidgetAction);
}

// Create Module menu with specified QAction as parent
void DUQWindow::createModuleMenu(QMenu* parent)
{
	QFont italicFont = parent->font();
	italicFont.setItalic(true);

	// General widgets, not associated to a module
	QAction* menuItem = parent->addAction("General");
	menuItem->setFont(italicFont);
	menuItem->setEnabled(false);
	menuItem = parent->addAction("PairPotential");
	connect(menuItem, SIGNAL(triggered(bool)), this, SLOT(addWidgetToCurrentWorkspace(bool)));

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
			connect(moduleItem, SIGNAL(triggered(bool)), this, SLOT(addWidgetToCurrentWorkspace(bool)));
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
		connect(moduleItem, SIGNAL(triggered(bool)), this, SLOT(addWidgetToCurrentWorkspace(bool)));
	}
}

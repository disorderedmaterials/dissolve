/*
	*** Browser Window - Functions
	*** src/gui/browser_funcs.cpp
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

#include "gui/browser.h"
#include "gui/mainwindow.h"
#include "gui/modulecontrolwidget.h"
#include "main/duq.h"
#include "classes/species.h"
#include "templates/variantpointer.h"
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QLabel>

// Constructor
BrowserWindow::BrowserWindow(MonitorWindow& monitorWindow, DUQ& duq) : duq_(duq), monitorWindow_(monitorWindow)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = false;

	updateWindow();
}

// Window close event
void BrowserWindow::closeEvent(QCloseEvent* event)
{
	hide();
	event->accept();
}

// Update window contents
void BrowserWindow::updateWindow()
{
	ui.BrowserTree->clear();

	QTreeWidgetItem* topItem, *item, *subItem;

	// Add on AtomTypes entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Atom Types");
	ui.BrowserTree->addTopLevelItem(topItem);

	// Add Species entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Species");
	ui.BrowserTree->addTopLevelItem(topItem);
	for (Species* sp = duq_.species(); sp != NULL; sp = sp->next)
	{
		item = new QTreeWidgetItem(BrowserWindow::SpeciesDataType);
		item->setText(0, sp->name());
		item->setData(0, Qt::UserRole, VariantPointer<Species>(sp));
		topItem->addChild(item);
	}

	// Add Configurations entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Configurations");
	ui.BrowserTree->addTopLevelItem(topItem);
	for (Configuration* cfg = duq_.configurations(); cfg != NULL; cfg = cfg->next)
	{
		item = new QTreeWidgetItem(BrowserWindow::ConfigurationDataType);
		item->setText(0, cfg->name());
		item->setData(0, Qt::UserRole, VariantPointer<Configuration>(cfg));
		topItem->addChild(item);

		// Add Module entries
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			subItem = new QTreeWidgetItem(BrowserWindow::ModuleDataType);
			subItem->setText(0, CharString("%s (%s)", module->name(), module->uniqueName()).get());
			subItem->setData(0, Qt::UserRole, VariantPointer<Module>(module));
			item->addChild(subItem);
		}
	}
}

/*
 * Widgets / Functions
 */

// Object tree double-clicked
void BrowserWindow::on_BrowserTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// Check the item pointer
	if (!item) return;

	QMdiSubWindow* window;

	printf("type = %i\n", item->type());
	// Now act on the data type of the item
	switch (item->type())
	{
		case (BrowserWindow::ConfigurationDataType):
			// Is the Configuration already displayed?
			window = monitorWindow_.currentWindow(item->data(0, Qt::UserRole).data());
			if (!window)
			{
				// Create the new Configuration viewer
				// TODO
			}
			else window->setFocus();
			break;
		case (BrowserWindow::ModuleDataType):
			// Is the Module already displayed?
			window = monitorWindow_.currentWindow(item->data(0, Qt::UserRole).data());
			if (!window)
			{
				Module* module = VariantPointer<Module>(item->data(0, Qt::UserRole));
				// Create a new ModuleWidget
				ModuleControlWidget* moduleControlWidget = new ModuleControlWidget(NULL, module);
				window = monitorWindow_.addWindow(moduleControlWidget, module, CharString("%s (%s)", module->name(), module->uniqueName()));
			}
			else window->setFocus();
			break;
		case (BrowserWindow::SpeciesDataType):
			break;
		default:
			Messenger::print("BrowserTree doesn't know what to do with data type '%i'\n", item->type());
	}
}

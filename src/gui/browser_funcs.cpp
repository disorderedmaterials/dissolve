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
#include "main/duq.h"
#include "classes/species.h"
#include <QCloseEvent>
#include <QLabel>

// Constructor
BrowserWindow::BrowserWindow(DUQ& duq) : duq_(duq)
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
		item = new QTreeWidgetItem();
		item->setText(0, sp->name());
		topItem->addChild(item);
	}

	// Add Configurations entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Configurations");
	ui.BrowserTree->addTopLevelItem(topItem);
	for (Configuration* cfg = duq_.configurations(); cfg != NULL; cfg = cfg->next)
	{
		item = new QTreeWidgetItem();
		item->setText(0, cfg->name());
		topItem->addChild(item);

		// Add Module entries
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			subItem = new QTreeWidgetItem();
			subItem->setText(0, CharString("%s (%s)", module->name(), module->uniqueName()).get());
			item->addChild(subItem);
		}
	}
}

/*
 * Widgets / Functions
 */


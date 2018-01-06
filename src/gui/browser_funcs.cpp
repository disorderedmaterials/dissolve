/*
	*** Browser Window - Functions
	*** src/gui/browser_funcs.cpp
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

#include "gui/browser.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/pairpotentialwidget.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QLabel>

// Constructor
BrowserWidget::BrowserWidget(QWidget* parent, DUQWindow& monitorWindow, DUQ& duq) : SubWidget(parent), duq_(duq), duqWindow_(monitorWindow)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = false;

	updateControls();
}

/*
 * SubWidget Reimplementations / Definitions
 */

// Window close event
void BrowserWidget::closeEvent(QCloseEvent* event)
{
	hide();
	event->accept();
}

// Update window contents
void BrowserWidget::updateControls()
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
		item = new QTreeWidgetItem(BrowserWidget::SpeciesDataType);
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
		item = new QTreeWidgetItem(BrowserWidget::ConfigurationDataType);
		item->setText(0, cfg->name());
		item->setData(0, Qt::UserRole, VariantPointer<Configuration>(cfg));
		topItem->addChild(item);

		// Add Module entries
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			subItem = new QTreeWidgetItem(BrowserWidget::ModuleDataType);
			subItem->setText(0, CharString("%s (%s)", module->name(), module->uniqueName()).get());
			subItem->setData(0, Qt::UserRole, VariantPointer<Module>(module));
			item->addChild(subItem);
		}
	}

	// Add PairPotentials entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Pair Potentials");
	ui.BrowserTree->addTopLevelItem(topItem);
	for (PairPotential* pp = duq_.pairPotentials(); pp != NULL; pp = pp->next)
	{
		item = new QTreeWidgetItem(BrowserWidget::PairPotentialType);
		item->setText(0, CharString("%s-%s", pp->atomTypeNameI(), pp->atomTypeNameJ()).get());
		item->setData(0, Qt::UserRole, VariantPointer<PairPotential>(pp));
		topItem->addChild(item);
	}

	// Add Processing Modules entry
	topItem = new QTreeWidgetItem();
	topItem->setText(0, "Processing");
	ui.BrowserTree->addTopLevelItem(topItem);
	RefListIterator<Module,bool> moduleIterator(duq_.processingModules().modules());
	while (Module* module = moduleIterator.iterate())
	{
		item = new QTreeWidgetItem(BrowserWidget::ModuleDataType);
		item->setText(0, CharString("%s (%s)", module->name(), module->uniqueName()).get());
		item->setData(0, Qt::UserRole, VariantPointer<Module>(module));
		topItem->addChild(item);
	}
}

// Disable sensitive controls within widget, ready for main code to run
void BrowserWidget::disableSensitiveControls()
{
// 	ui.BrowserTree->setEnabled(false);
}

// Enable sensitive controls within widget, ready for main code to run
void BrowserWidget::enableSensitiveControls()
{
// 	ui.BrowserTree->setEnabled(true);
}

// Return string specifying widget type
const char* BrowserWidget::widgetType()
{
	return "Browser";
}

// Write widget state through specified LineParser
bool BrowserWidget::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool BrowserWidget::readState(LineParser& parser)
{
	return true;
}

/*
 * Widgets / Functions
 */

// Object tree double-clicked
void BrowserWidget::on_BrowserTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// Check the item pointer
	if (!item) return;

	QMdiSubWindow* window;
	SubWidget* subWidget = NULL;

	// Now act on the data type of the item
	switch (item->type())
	{
		case (BrowserWidget::ConfigurationDataType):
			// Is the Configuration already displayed?
			window = duqWindow_.currentWindow(item->data(0, Qt::UserRole).data());
			if (!window)
			{
				// Create the new Configuration viewer
				// TODO
			}
			else window->setFocus();
			break;
		case (BrowserWidget::ModuleDataType):
			// Is the Module already displayed?
			window = duqWindow_.currentWindow(item->data(0, Qt::UserRole).data());
			if (!window)
			{
				Module* module = VariantPointer<Module>(item->data(0, Qt::UserRole));
				// Create a new ModuleWidget
				ModuleControlWidget* moduleControlWidget = new ModuleControlWidget(NULL, module, duq_);
				connect(moduleControlWidget, SIGNAL(moduleRun()), &duqWindow_, SLOT(updateWidgets()));
				subWidget = moduleControlWidget;
				window = duqWindow_.addWindow(moduleControlWidget, module, CharString("%s (%s)", module->name(), module->uniqueName()));
			}
			else window->setFocus();
			break;
		case (BrowserWidget::PairPotentialType):
			// Is the Module already displayed?
			window = duqWindow_.currentWindow(item->data(0, Qt::UserRole).data());
			if (!window)
			{
				PairPotential* pp = VariantPointer<PairPotential>(item->data(0, Qt::UserRole));
				// Create a new PairPotentialWidget
				PairPotentialWidget* pairPotentialWidget = new PairPotentialWidget(NULL, pp, duq_);
				window = duqWindow_.addWindow(pairPotentialWidget, pp, CharString("Pair Potential %s-%s", pp->atomTypeNameI(), pp->atomTypeNameJ()));
				subWidget = pairPotentialWidget;
			}
			else window->setFocus();
			break;
		case (BrowserWidget::SpeciesDataType):
			break;
		default:
			Messenger::print("BrowserTree doesn't know what to do with data type '%i'\n", item->type());
	}

	// If we created a new subwidget, make sure it is created in the correct 'state'
	if (subWidget)
	{
		// If we are currently running, make sure any sensitive controls in the widget are disabled
		if (duqWindow_.duqState() != DUQWindow::StoppedState) subWidget->disableSensitiveControls();
	}
}

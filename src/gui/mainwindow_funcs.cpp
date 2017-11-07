/*
	*** Main Window - Functions
	*** src/gui/mainwindow_funcs.cpp
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
#include "gui/mainwindow.h"
#include "gui/modulecontrolwidget.h"
#include <QCloseEvent>
#include <QTabWidget>

// Constructor
MonitorWindow::MonitorWindow(DUQ& duq) : QMainWindow(NULL), duq_(duq)
{
	// Initialise Qt's icon resource
	Q_INIT_RESOURCE(icons);

	// Set up user interface
	ui.setupUi(this);

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
	/* if (saveBeforeClose())
	{
		saveSettings();
		event->accept();
	}
	else event->ignore();
	*/
	event->accept();
}

void MonitorWindow::resizeEvent(QResizeEvent* event)
{
}

/*
 * Update Functions
 */

// Set up window after load
void MonitorWindow::setUp()
{
	// Add controls for all processing modules
	RefListIterator<Module,bool> moduleIterator(duq_.processingModules().modules());
	while (Module* module = moduleIterator.iterate())
	{
		// Create a new ModuleWidget to use as the page for the tab
		ModuleControlWidget* moduleControlWidget = new ModuleControlWidget(NULL, module);
		ui.ProcessingModulesTab->addTab(moduleControlWidget, CharString("%s (%s)", module->name(), module->uniqueName()).get());
		
	}
}

// Refresh specified aspects of the window
void MonitorWindow::updateWidgets(int targets)
{
	// Update Species tab...
	// TODO

	// Update Configurations & Modules tab...
	// TODO

	// Update Processing Modules tab...
	// TODO

	// Update Data Sources tab...
	// TODO

	// Update Pair Potentials tab...
	// TODO
}

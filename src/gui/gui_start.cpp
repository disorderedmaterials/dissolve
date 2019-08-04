/*
	*** Dissolve GUI - Start Page Functions
	*** src/gui/gui_start.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/gui.h"
#include "main/dissolve.h"
#include <QDirIterator>

/*
 * Create
 */

// Add system templates to the Create group
void DissolveWindow::addSystemTemplates()
{
	// Probe our main resource object for the templates, and create local data from them
	QDirIterator templateIterator(":/data/systemtemplates/");
	while (templateIterator.hasNext())
	{
		QDir dir = templateIterator.next();

		// Open the associated xml file
		SystemTemplate* sysTemp = systemTemplates_.add();
		if (!sysTemp->read(dir))
		{
			Messenger::error("Error reading the template info file '%s'.\n", qPrintable(dir.filePath("info.xml")));
			systemTemplates_.remove(sysTemp);
			continue;
		}

		// Create a button for this template and add it to the layout
		QToolButton* toolButton = sysTemp->createButton();
		ui.StartCreateScrollAreaContents->layout()->addWidget(toolButton);
		connect(toolButton, SIGNAL(clicked(bool)), this, SLOT(systemTemplateClicked(bool)));
	}
}

void DissolveWindow::on_StartCreateNewButton_clicked(bool checked)
{
	ui.SessionNewAction->trigger();
}

void DissolveWindow::systemTemplateClicked(bool checked)
{
	QToolButton* toolButton = qobject_cast<QToolButton*>(sender());
	if (!toolButton) return;

	// Find the SystemTemplate that links to this button
	ListIterator<SystemTemplate> templateIterator(systemTemplates_);
	while (SystemTemplate* sysTemp = templateIterator.iterate()) if (sysTemp->toolButton() == toolButton)
	{
		// Make sure there is nothing to save before we load the template input
		if (!checkSaveCurrentInput()) return;

		// Clear any data-related tabs from the UI
		clearTabs();

		// Load the input data
		if (!dissolve_.loadInputFromString(qPrintable(sysTemp->inputFileData())))
		{
			startNew();
			return;
		}

		localSimulation_ = true;

		// Fully update GUI
		fullUpdate();

		// Make sure we are now on the Simulation stack page
		showMainStackPage(DissolveWindow::SimulationStackPage);
	}
}

/*
 * Open / Connect
 */

void DissolveWindow::on_StartOpenLocalButton_clicked(bool checked)
{
	ui.SessionOpenLocalAction->trigger();
}

void DissolveWindow::on_StartOpenRecentButton_clicked(bool checked)
{
	ui.SessionOpenRecentAction->trigger();
}

void DissolveWindow::on_StartConnectButton_clicked(bool checked)
{
	ui.SessionConnectAction->trigger();
}

/*
 * Help
 */

void DissolveWindow::on_StartOnlineManualButton_clicked(bool checked)
{
	ui.HelpOnlineManualAction->trigger();
}

void DissolveWindow::on_StartOnlineTutorialsButton_clicked(bool checked)
{
	ui.HelpOnlineTutorialsAction->trigger();
}

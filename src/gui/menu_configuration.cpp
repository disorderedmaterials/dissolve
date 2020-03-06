/*
	*** Dissolve GUI - Configuration Menu Functions
	*** src/gui/menu_configuration.cpp
	Copyright T. Youngs 2012-2020

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
#include "gui/configurationtab.h"
#include "gui/selectspeciesdialog.h"
#include "main/dissolve.h"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/parameters.h"
#include "io/export/coordinates.h"
#include <QFileDialog>
#include <QMessageBox>

void DissolveWindow::on_ConfigurationCreateEmptyAction_triggered(bool checked)
{
	Configuration* newConfiguration = dissolve_.addConfiguration();

	setModified();
	fullUpdate();
	ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

	RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
	if (mixSpecies.nItems() == 0) return;

	// Create the Configuration and a suitable generator
	Configuration* newConfiguration = dissolve_.addConfiguration();
	Procedure& generator = newConfiguration->generator();
	ParametersProcedureNode* paramsNode = new ParametersProcedureNode;
	paramsNode->addParameter("rho", 0.1);
	generator.addRootSequenceNode(paramsNode);
	generator.addRootSequenceNode(new BoxProcedureNode);
	for(auto sp : mixSpecies)
	{
		generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, 100, NodeValue("rho", paramsNode->parameterReferences())));
	}

	// Run the generator
	newConfiguration->generate(dissolve_.worldPool(), dissolve_.pairPotentialRange());

	setModified();
	fullUpdate();
	ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

	RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
	if (mixSpecies.nItems() == 0) return;

	// Create the Configuration and a suitable generator
	Configuration* newConfiguration = dissolve_.addConfiguration();
	Procedure& generator = newConfiguration->generator();
	ParametersProcedureNode* paramsNode = new ParametersProcedureNode;
	paramsNode->addParameter("populationA", 100);
	paramsNode->addParameter("rho", 0.1);
	generator.addRootSequenceNode(paramsNode);
	generator.addRootSequenceNode(new BoxProcedureNode);
	int count = 0;
	for (Species* sp : mixSpecies)
	{
		// Add a parameter for the ratio of this species to the first (or the population of the first)
		if (count==0) generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue("populationA", paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		else
		{
			CharString parameterName("ratio%c", 65+count);
			paramsNode->addParameter(parameterName, 1);

			generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue(CharString("%s*populationA", parameterName.get()), paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		}

		++count;
	}

	// Run the generator
	newConfiguration->generate(dissolve_.worldPool(), dissolve_.pairPotentialRange());

	setModified();
	fullUpdate();
	ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a ConfigurationTab, then call its rename() function
	MainTab* tab = ui_.MainTabs->currentTab();
	if ((!tab) || (tab->type() != MainTab::ConfigurationTabType)) return;
	tab->rename();
}

void DissolveWindow::on_ConfigurationDeleteAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a ConfigurationTab
	MainTab* tab = ui_.MainTabs->currentTab();
	if ((!tab) || (tab->type() != MainTab::ConfigurationTabType)) return;

	// Cast up the tab to a ConfigurationTab so we can get the ModuleLayer pointer
	ConfigurationTab* cfgTab = dynamic_cast<ConfigurationTab*>(tab);
	if (!cfgTab) return;

	// Check that we really want to delete the Configuration
	if (!cfgTab->close()) return;

	// Update the GUI
	ui_.MainTabs->removeByPage(cfgTab->page());
	setModified();
	fullUpdate();
}

void DissolveWindow::on_ConfigurationExportToXYZAction_triggered(bool checked)
{
	// Get the currently-displayed Configuration
	Configuration* cfg = ui_.MainTabs->currentConfiguration();
	if (!cfg) return;

	// Get a suitable export file name
	QString exportFile = QFileDialog::getSaveFileName(this, "Choose XYZ export file name", QDir().absolutePath(), "XYZ Coordinates (*.xyz)");
	if (exportFile.isEmpty()) return;

	CoordinateExportFileFormat fileAndFormat(qPrintable(exportFile), CoordinateExportFileFormat::XYZCoordinates);
	if (!fileAndFormat.exportData(cfg)) QMessageBox::warning(this, "Error", "Failed to export the configuration. Check the messages for details.", QMessageBox::Ok, QMessageBox::Ok);
	else Messenger::print("Successfully exported configuration '%s' to '%s'.\n", cfg->name(), fileAndFormat.filename());
}

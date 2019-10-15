/*
	*** Dissolve GUI - Configuration Menu Functions
	*** src/gui/menu_configuration.cpp
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
#include "gui/selectspeciesdialog.h"
#include "main/dissolve.h"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/parameters.h"

void DissolveWindow::on_ConfigurationCreateEmptyAction_triggered(bool checked)
{
	Configuration* newConfiguration = dissolve_.addConfiguration();

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	static SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

	RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
	if (mixSpecies.nItems() == 0) return;

	// Create the Configuration and a suitable generator
	Configuration* newConfiguration = dissolve_.addConfiguration();
	Procedure& generator = newConfiguration->generator();
	ParametersProcedureNode* paramsNode = new ParametersProcedureNode;
	paramsNode->addParameter("rho", 0.1);
	generator.addRootSequenceNode(paramsNode);
	generator.addRootSequenceNode(new BoxProcedureNode);
	RefListIterator<Species> mixIterator(mixSpecies);
	while (Species* sp = mixIterator.iterate())
	{
		generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, 100, NodeValue("rho", paramsNode->parameterReferences())));
	}

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	static SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

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
	RefListIterator<Species> mixIterator(mixSpecies);
	int count = 0;
	while (Species* sp = mixIterator.iterate())
	{
		// Add a parameter for the ratio of this species to the first (or the population of the first)
		if (mixIterator.isFirst()) generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue("populationA", paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		else
		{
			CharString parameterName("ratio%c", 65+count);
			paramsNode->addParameter(parameterName, 1);

			generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue(CharString("%s*populationA", parameterName.get()), paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		}

		++count;
	}

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a ConfigurationTab, then call its rename() function
	MainTab* tab = currentTab();
	if ((!tab) || (tab->type() != MainTab::ConfigurationTabType)) return;
	tab->rename();
}

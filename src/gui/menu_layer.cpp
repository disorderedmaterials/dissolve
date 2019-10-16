/*
	*** Dissolve GUI - Layer Menu Functions
	*** src/gui/menu_layer.cpp
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
#include "gui/selectconfigurationdialog.h"
#include "main/dissolve.h"

void DissolveWindow::on_LayerCreateEmptyAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolutionStandardAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	Module* module;

	// Add a Monte Carlo shake (MolShake) module
	module = dissolve_.createModuleInstance("MolShake", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());

	// Add some MD
	module = dissolve_.createModuleInstance("MD", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());
	module->setFrequency(5);

	// Add energy calculation
	module = dissolve_.createModuleInstance("Energy", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolutionEPSRAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	Module* module;

	// Add some Monte Carlo
	module = dissolve_.createModuleInstance("MolShake", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());

	// Add an intramolecular shake
	module = dissolve_.createModuleInstance("IntraShake", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());

	// Add energy calculation
	module = dissolve_.createModuleInstance("Energy", newLayer);
	module->addTargetConfigurations(dissolve_.configurations());

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateRefinementEPSRAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	// Add the EPSR module
	dissolve_.createModuleInstance("EPSR", newLayer);

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a LayerTab, then call its rename() function
	MainTab* tab = currentTab();
	if ((!tab) || (tab->type() != MainTab::LayerTabType)) return;
	tab->rename();
}

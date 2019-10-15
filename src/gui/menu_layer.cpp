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
#include "main/dissolve.h"

void DissolveWindow::on_LayerCreateEvolutionStandardAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	// Add a Monte Carlo shake (MolShake) module
	dissolve_.createModuleInstance("MolShake", newLayer);

	// Add some MD
	Module* md = dissolve_.createModuleInstance("MD", newLayer);
	md->setFrequency(5);

	// Add energy calculation
	dissolve_.createModuleInstance("Energy", newLayer);

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolutionEPSRAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	// Add some Monte Carlo
	dissolve_.createModuleInstance("MolShake", newLayer);

	// Add an intramolecular shake
	dissolve_.createModuleInstance("IntraShake", newLayer);

	// Add energy calculation
	dissolve_.createModuleInstance("Energy", newLayer);

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

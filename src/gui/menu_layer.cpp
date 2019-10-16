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
#include "gui/layertab.h"
#include "main/dissolve.h"
#include <QMessageBox>

void DissolveWindow::on_LayerCreateEmptyAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();

	setModified();
	fullUpdate();
	setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolutionMolecularAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();
	newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Standard)"));

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

void DissolveWindow::on_LayerCreateEvolutionAtomicAction_triggered(bool checked)
{
	ModuleLayer* newLayer = dissolve_.addProcessingLayer();
	newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Atomic)"));

	Module* module;

	// Add some Monte Carlo
	module = dissolve_.createModuleInstance("AtomShake", newLayer);
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
	newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (EPSR)"));

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
	newLayer->setName(dissolve_.uniqueProcessingLayerName("Refine (EPSR)"));

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

void DissolveWindow::on_LayerDeleteAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a LayerTab
	MainTab* tab = currentTab();
	if ((!tab) || (tab->type() != MainTab::LayerTabType)) return;

	// Check that we really want to delete this tab
	QMessageBox queryBox;
	queryBox.setText(QString("Really delete the layer '%1'?").arg(tab->title()));
	queryBox.setInformativeText("Proceed?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Yes)
	{
		// Cast up the tab to a LayerTab so we can get the ModuleLayer pointer
		LayerTab* layerTab = dynamic_cast<LayerTab*>(tab);
		if (!layerTab) return;
		ModuleLayer* layer = layerTab->moduleLayer();

		// Remove the tab
		removeTab(layerTab);

		// Remove the layer
		dissolve_.removeProcessingLayer(layer);

		// Update the GUI
		setModified();
		fullUpdate();
	}
}

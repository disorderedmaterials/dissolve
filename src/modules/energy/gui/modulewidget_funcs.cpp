/*
	*** Energy Module Widget - Functions
	*** src/modules/energy/gui/modulewidget_funcs.cpp
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

#include "modules/energy/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/mimetreewidgetitem.h"
#include "main/duq.h"
#include "modules/energy/energy.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
EnergyModuleWidget::EnergyModuleWidget(QWidget* parent, Module* module, DUQ& dUQ) : ModuleWidget(parent), module_((EnergyModule*) module), dUQ_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Add UChromaWidget to the PlotWidget
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	energyGraph_ = new UChromaViewWidget;
	layout->addWidget(energyGraph_);
	ui.PlotWidget->setLayout(layout);

	// Start a new, empty session
	energyGraph_->startNewSession(true);
	ViewPane* viewPane = energyGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "Iteration");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().numberFormat(0).setNDecimals(0);
	viewPane->axes().setTitle(1, "Energy, kJ mol\\sup{-1}");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->axes().numberFormat(0).setUseENotation(true);

	initialiseControls(module_);

	refreshing_ = false;
}

EnergyModuleWidget::~EnergyModuleWidget()
{
}

// Update controls within widget
void EnergyModuleWidget::updateControls()
{
	// Ensure that any displayed data is up-to-date
	energyGraph_->refreshReferencedDataSets();

	energyGraph_->updateDisplay();
}

// Initialise controls
void EnergyModuleWidget::initialiseControls(EnergyModule* module)
{
	if (!module) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool EnergyModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!energyGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool EnergyModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!energyGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

void EnergyModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current collections
	energyGraph_->clearCollections();

	// Get target Configuration
	Configuration* cfg = (Configuration*) VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
	if (!cfg) return;

	// Set data targets
	CharString blockData;
	blockData.sprintf("Collection 'Total'; Group 'Total'; LineStyle 1.0 Solid; DataSet 'Total'; Source XYData 'XYData@%s//%s//Total'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Inter'; Group 'Inter'; LineStyle 1.0 Solid; DataSet 'Inter'; Source XYData 'XYData@%s//%s//Inter'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Intra'; Group 'Intra'; LineStyle 1.0 Solid; DataSet 'Intra'; Source XYData 'XYData@%s//%s//Intra'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Bond'; Group 'Bond'; LineStyle 1.0 Solid; DataSet 'Bond'; Source XYData 'XYData@%s//%s//Bond'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Angle'; Group 'Angle'; LineStyle 1.0 Solid; DataSet 'Angle'; Source XYData 'XYData@%s//%s//Angle'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Torsion'; Group 'Torsion'; LineStyle 1.0 Solid; DataSet 'Torsion'; Source XYData 'XYData@%s//%s//Torsion'; EndDataSet; EndCollection", cfg->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
}

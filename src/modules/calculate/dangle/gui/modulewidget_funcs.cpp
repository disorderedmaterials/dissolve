/*
	*** CalculateRDF Module Widget - Functions
	*** src/modules/calculate/dangle/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/calculate/dangle/gui/modulewidget.h"
#include "modules/calculate/dangle/dangle.h"
#include "classes/configuration.h"

// Constructor
CalculateDAngleModuleWidget::CalculateDAngleModuleWidget(QWidget* parent, CalculateDAngleModule* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui.setupUi(this);

	// Set up RDF graph
	rdfGraph_ = ui.RDFPlotWidget;

	rdfGraph_->startNewSession(true);
	ViewPane* viewPane = rdfGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, 0.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	setGraphDataTargets(module_);

	refreshing_ = false;
}

// Update controls within widget
void CalculateDAngleModuleWidget::updateControls()
{
	rdfGraph_->refreshReferencedDataSets();

	rdfGraph_->updateDisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void CalculateDAngleModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void CalculateDAngleModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool CalculateDAngleModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!rdfGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool CalculateDAngleModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!rdfGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateDAngleModuleWidget::setGraphDataTargets(CalculateDAngleModule* module)
{
	// Remove any current collections
	rdfGraph_->clearCollections();

	printf("LKjlkjklj\n");
	// Loop over Configuration targets in Module
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		// Calculated RDF
		CharString blockData;
		blockData.sprintf("Collection '%s'; Group '%s'; DataSet '%s (%s)'; Source Data1D '%s//Process1D//%s//%s'; EndDataSet; EndCollection", cfg->name(), cfg->name(), module_->keywords().asString("Name"), cfg->name(), module_->uniqueName(), cfg->niceName(), module_->rdfName());
		printf("BLOCK = [%s]\n", blockData.get());
		rdfGraph_->addCollectionFromBlock(blockData);
	}
}

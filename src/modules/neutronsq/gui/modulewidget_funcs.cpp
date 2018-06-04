/*
	*** Neutron SQ Module Widget - Functions
	*** src/modules/neutronsq/gui/modulewidget_funcs.cpp
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

#include "modules/neutronsq/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/neutronsq/neutronsq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
NeutronSQModuleWidget::NeutronSQModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((NeutronSQModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	ViewPane* viewPane;

	// Set up partial g(r) graph

	partialGRGraph_ = ui.PartialGRPlotWidget;

	partialGRGraph_->startNewSession(true);
	viewPane = partialGRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Set up partial S(Q) graph

	partialSQGraph_ = ui.PartialSQPlotWidget;

	partialSQGraph_->startNewSession(true);
	viewPane = partialSQGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Set up total G(r) graph

	totalGRGraph_ = ui.TotalGRPlotWidget;

	totalGRGraph_->startNewSession(true);
	viewPane = totalGRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::NoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Set up total S(Q) graph

	totalSQGraph_ = ui.TotalSQPlotWidget;

	totalSQGraph_->startNewSession(true);
	viewPane = totalSQGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::NoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	setGraphDataTargets(module_);

	refreshing_ = false;
}

NeutronSQModuleWidget::~NeutronSQModuleWidget()
{
}

// Update controls within widget
void NeutronSQModuleWidget::updateControls()
{
	// Ensure that any displayed data are up-to-date
	partialGRGraph_->refreshReferencedDataSets();
	partialSQGraph_->refreshReferencedDataSets();
	totalGRGraph_->refreshReferencedDataSets();
	totalSQGraph_->refreshReferencedDataSets();

	partialGRGraph_->updateDisplay();
	partialSQGraph_->updateDisplay();
	totalGRGraph_->updateDisplay();
	totalSQGraph_->updateDisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void NeutronSQModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void NeutronSQModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool NeutronSQModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!partialGRGraph_->writeSession(parser)) return false;
	if (!partialSQGraph_->writeSession(parser)) return false;
	if (!totalGRGraph_->writeSession(parser)) return false;
	if (!totalSQGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool NeutronSQModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!partialGRGraph_->readSession(parser)) return false;
	if (!partialSQGraph_->readSession(parser)) return false;
	if (!totalGRGraph_->readSession(parser)) return false;
	if (!totalSQGraph_->readSession(parser)) return false;
	
	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void NeutronSQModuleWidget::setGraphDataTargets(NeutronSQModule* module)
{
	CharString blockData;

	// Add partials
	int n = 0;
	for (AtomType* at1 = dissolve_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			/*
			 * Partial g(r)
			 */

			// Full partial
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet 'Calculated %s'; Source XYData '%s//WeightedGR//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			// Bound partial
			blockData.sprintf("Collection '%s Bound'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated %s'; Source XYData '%s//WeightedGR//%s-%s//Bound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			// Unbound partial
			blockData.sprintf("Collection '%s Unbound'; Group '%s'; LineStyle 1.0 Dots; DataSet 'Calculated %s'; Source XYData '%s//WeightedGR//%s-%s//Unbound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			/*
			 * Partial S(Q)
			 */

			// Full partial
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet 'Calculated %s'; Source XYData '%s//WeightedSQ//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Bound partial
			blockData.sprintf("Collection '%s Bound'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated %s'; Source XYData '%s//WeightedSQ//%s-%s//Bound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Unbound partial
			blockData.sprintf("Collection '%s Unbound'; Group '%s'; LineStyle 1.0 Dots; DataSet 'Calculated %s'; Source XYData '%s//WeightedSQ//%s-%s//Unbound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Add calculated total G(r)
	blockData.sprintf("Collection 'G(r)'; Group 'Calc'; DataSet 'Calculated'; Source XYData '%s//WeightedGR//Total'; EndDataSet; EndCollection", module_->uniqueName());
	totalGRGraph_->addCollectionFromBlock(blockData);

	// Add calculate total F(Q)
	blockData.sprintf("Collection 'F(Q)'; Group 'Calc'; DataSet 'Calculated'; Source XYData '%s//WeightedSQ//Total'; EndDataSet; EndCollection", module_->uniqueName());
	totalSQGraph_->addCollectionFromBlock(blockData);

	// Add on reference data if present
	if (module->keywords().find("Reference"))
	{
		// Add FT of reference data total G(r)
		blockData.sprintf("Collection 'Reference'; Group 'Reference'; DataSet 'Reference'; Source XYData '%s//ReferenceDataFT'; EndDataSet; EndCollection", module_->uniqueName());
		totalGRGraph_->addCollectionFromBlock(blockData);

		// Add calculate total F(Q)
		blockData.sprintf("Collection 'Reference'; Group 'Reference'; DataSet 'Reference'; Source XYData '%s//ReferenceData'; EndDataSet; EndCollection", module_->uniqueName());
		totalSQGraph_->addCollectionFromBlock(blockData);
	}
}

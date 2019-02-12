/*
	*** Neutron SQ Module Widget - Functions
	*** src/modules/neutronsq/gui/modulewidget_funcs.cpp
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

#include "modules/neutronsq/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
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

	partialGRGraph_ = ui.PartialGRPlotWidget->dataViewer();

	partialGRGraph_->startNewSession(true);
	partialGRGraph_->view().setViewType(View::FlatXYView);
	partialGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	partialGRGraph_->view().axes().setMax(0, 10.0);
	partialGRGraph_->view().axes().setTitle(1, "g(r)");
	partialGRGraph_->view().axes().setMin(1, -1.0);
	partialGRGraph_->view().axes().setMax(1, 1.0);
	partialGRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up partial S(Q) graph

	partialSQGraph_ = ui.PartialSQPlotWidget->dataViewer();

	partialSQGraph_->startNewSession(true);
	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph

	totalGRGraph_ = ui.TotalGRPlotWidget->dataViewer();

	totalGRGraph_->startNewSession(true);
	totalGRGraph_->view().setViewType(View::FlatXYView);
	totalGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	totalGRGraph_->view().axes().setMax(0, 10.0);
	totalGRGraph_->view().axes().setTitle(1, "g(r)");
	totalGRGraph_->view().axes().setMin(1, -1.0);
	totalGRGraph_->view().axes().setMax(1, 1.0);
	totalGRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::NoVerticalShift);
	totalGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total S(Q) graph

	totalSQGraph_ = ui.TotalSQPlotWidget->dataViewer();

	totalSQGraph_->startNewSession(true);
	totalSQGraph_->view().setViewType(View::FlatXYView);
	totalSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	totalSQGraph_->view().axes().setMax(0, 10.0);
	totalSQGraph_->view().axes().setTitle(1, "S(Q)");
	totalSQGraph_->view().axes().setMin(1, -1.0);
	totalSQGraph_->view().axes().setMax(1, 1.0);
	totalSQGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::NoVerticalShift);
	totalSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

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

	partialGRGraph_->postRedisplay();
	partialSQGraph_->postRedisplay();
	totalGRGraph_->postRedisplay();
	totalSQGraph_->postRedisplay();
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
	// Write DataViewer sessions
	if (!partialGRGraph_->writeSession(parser)) return false;
	if (!partialSQGraph_->writeSession(parser)) return false;
	if (!totalGRGraph_->writeSession(parser)) return false;
	if (!totalSQGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool NeutronSQModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
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
	for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			/*
			 * Partial g(r)
			 */

			// Full partial
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet 'Calculated %s'; Source Data1D '%s//WeightedGR//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			// Bound partial
			blockData.sprintf("Collection '%s Bound'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated %s'; Source Data1D '%s//WeightedGR//%s-%s//Bound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			// Unbound partial
			blockData.sprintf("Collection '%s Unbound'; Group '%s'; LineStyle 1.0 Dots; DataSet 'Calculated %s'; Source Data1D '%s//WeightedGR//%s-%s//Unbound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialGRGraph_->addCollectionFromBlock(blockData);

			/*
			 * Partial S(Q)
			 */

			// Full partial
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet 'Calculated %s'; Source Data1D '%s//WeightedSQ//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Bound partial
			blockData.sprintf("Collection '%s Bound'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated %s'; Source Data1D '%s//WeightedSQ//%s-%s//Bound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Unbound partial
			blockData.sprintf("Collection '%s Unbound'; Group '%s'; LineStyle 1.0 Dots; DataSet 'Calculated %s'; Source Data1D '%s//WeightedSQ//%s-%s//Unbound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Add calculated total G(r)
	blockData.sprintf("Collection 'G(r)'; Group 'Calc'; DataSet 'Calculated'; Source Data1D '%s//WeightedGR//Total'; EndDataSet; EndCollection", module_->uniqueName());
	totalGRGraph_->addCollectionFromBlock(blockData);

	// Add calculate total F(Q)
	blockData.sprintf("Collection 'F(Q)'; Group 'Calc'; DataSet 'Calculated'; Source Data1D '%s//WeightedSQ//Total'; EndDataSet; EndCollection", module_->uniqueName());
	totalSQGraph_->addCollectionFromBlock(blockData);

	// Add on reference data if present
	if (module->keywords().find("Reference"))
	{
		// Add FT of reference data total G(r)
		blockData.sprintf("Collection 'Reference'; Group 'Reference'; DataSet 'Reference'; Source Data1D '%s//ReferenceDataFT'; EndDataSet; EndCollection", module_->uniqueName());
		totalGRGraph_->addCollectionFromBlock(blockData);

		// Add calculate total F(Q)
		blockData.sprintf("Collection 'Reference'; Group 'Reference'; DataSet 'Reference'; Source Data1D '%s//ReferenceData'; EndDataSet; EndCollection", module_->uniqueName());
		totalSQGraph_->addCollectionFromBlock(blockData);
	}
}

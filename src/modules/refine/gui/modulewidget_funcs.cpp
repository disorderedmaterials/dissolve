/*
	*** Refine Module Widget - Functions
	*** src/modules/refine/gui/modulewidget_funcs.cpp
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

#include "modules/refine/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/refine/refine.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
RefineModuleWidget::RefineModuleWidget(QWidget* parent, Module* module, Dissolve& Dissolve) : ModuleWidget(parent), module_((RefineModule*) module), Dissolve_(Dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = true;

	ViewPane* viewPane;

	// Data Graph

	dataGraph_ = ui.DataPlotWidget;

	// Start a new, empty session
	dataGraph_->startNewSession(true);
	viewPane = dataGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Partial S(Q) Graph
	
	partialSQGraph_ = ui.PartialSQPlotWidget;

	// Start a new, empty session
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

	// Partial g(r) Graph
	
	partialGRGraph_ = ui.PartialGRPlotWidget;

	// Start a new, empty session
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

	// Delta phi(r) Graph

	deltaPhiRGraph_ = ui.DeltaPhiRPlotWidget;

	// Start a new, empty session
	deltaPhiRGraph_->startNewSession(true);
	viewPane = deltaPhiRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "\\sym{Delta}g(r), \\sym{Delta}\\sym{phi}(\\it{r})");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Delta phi(r) Graph

	phiMagGraph_ = ui.PhiMagPlotWidget;

	// Start a new, empty session
	phiMagGraph_->startNewSession(true);
	viewPane = phiMagGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "Iteration");
	viewPane->axes().numberFormat(0).setNDecimals(0);
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "\\sym{Delta}\\sym{phi}(\\it{r}), kJ mol\\sup{-1} \\sum{angstrom}\\sup{-1}");
	viewPane->axes().setMin(1, 0.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->setAutoFollowType(ViewPane::XFollow);

	// Errors Graph

	errorsGraph_ = ui.ErrorsPlotWidget;

	// Start a new, empty session
	errorsGraph_->startNewSession(true);
	viewPane = errorsGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "Iteration");
	viewPane->axes().numberFormat(0).setNDecimals(0);
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "%Error");
	viewPane->axes().setMin(1, 0.0);
	viewPane->axes().setMax(1, 100.0);
	viewPane->setAutoFollowType(ViewPane::XFollow);

	setGraphDataTargets(module_);

	refreshing_ = false;
}

RefineModuleWidget::~RefineModuleWidget()
{
}

// Update controls within widget
void RefineModuleWidget::updateControls()
{
	refreshing_ = true;

	// Set controls on Overview page
	double phiLevel = 0.0;
	ListIterator<PairPotential> ppIterator(Dissolve_.pairPotentials());
	while (PairPotential* pp = ppIterator.iterate()) phiLevel += pp->uAdditional().absIntegral();
	ui.PhiLevelSpin->setValue(phiLevel);

	// Ensure that any displayed data are up-to-date
	dataGraph_->refreshReferencedDataSets();
	partialSQGraph_->refreshReferencedDataSets();
	partialGRGraph_->refreshReferencedDataSets();
	deltaPhiRGraph_->refreshReferencedDataSets();
	phiMagGraph_->refreshReferencedDataSets();
	errorsGraph_->refreshReferencedDataSets();

	dataGraph_->updateDisplay();
	partialSQGraph_->updateDisplay();
	partialGRGraph_->updateDisplay();
	deltaPhiRGraph_->updateDisplay();
	phiMagGraph_->updateDisplay();
	errorsGraph_->updateDisplay();

	refreshing_ = false;
}

// Disable sensitive controls within widget, ready for main code to run
void RefineModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void RefineModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool RefineModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!dataGraph_->writeSession(parser)) return false;
	if (!partialSQGraph_->writeSession(parser)) return false;
	if (!partialGRGraph_->writeSession(parser)) return false;
	if (!deltaPhiRGraph_->writeSession(parser)) return false;
	if (!phiMagGraph_->writeSession(parser)) return false;
	if (!errorsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool RefineModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!dataGraph_->readSession(parser)) return false;
	if (!partialSQGraph_->readSession(parser)) return false;
	if (!partialGRGraph_->readSession(parser)) return false;
	if (!deltaPhiRGraph_->readSession(parser)) return false;
	if (!phiMagGraph_->readSession(parser)) return false;
	if (!errorsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void RefineModuleWidget::setGraphDataTargets(RefineModule* module)
{
	if (!module) return;

	const int nTypes = Dissolve_.atomTypeList().nItems();
	int n, m;
	CharString blockData;

	// Loop over groups
	for (ModuleGroup* group = module_->targetGroups().first(); group != NULL; group = group->next)
	{
		// Add reference data & calculated data to the dataGraph_, and percentage errors to the errorsGraph_
		RefListIterator<Module,bool> targetIterator(module->targets());
		while (Module* targetModule = targetIterator.iterate())
		{
			// Reference data
			blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source XYData '%s//ReferenceData'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
			dataGraph_->addCollectionFromBlock(blockData);

			// Calculated data from associated module
			if (DissolveSys::sameString(targetModule->name(), "NeutronSQ"))
			{
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source XYData '%s//WeightedSQ//Total'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
				dataGraph_->addCollectionFromBlock(blockData);

				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle  1.0 'Dots'; DataSet '%s Error'; Source XYData '%s//Difference//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
				dataGraph_->addCollectionFromBlock(blockData);

				blockData.sprintf("Collection '%s Calc'; Group '%s'; DataSet '%s Error'; Source XYData '%s//Error//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
				errorsGraph_->addCollectionFromBlock(blockData);
			}
		}

		// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the partialSQGraph_
		n = 0;
		for (AtomType* at1 = Dissolve_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
		{
			m = n;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
			{
				CharString id("%s-%s/%s", at1->name(), at2->name(), group->name());

				/*
				 * Partial Structure Factors
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source XYData '%s//GeneratedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialSQGraph_->addCollectionFromBlock(blockData);

				// Calculated / summed partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source XYData '%s//UnweightedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialSQGraph_->addCollectionFromBlock(blockData);

				// Deltas
				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle 1.0 'Dots'; DataSet 'Delta %s'; Source XYData '%s//DeltaSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialSQGraph_->addCollectionFromBlock(blockData);

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source XYData '%s//GeneratedGR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialGRGraph_->addCollectionFromBlock(blockData);

				// Calculated partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source XYData '%s//UnweightedGR//%s//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialGRGraph_->addCollectionFromBlock(blockData);

				/*
				 * Phi(r)
				 */

				// Generated potential
				blockData.sprintf("Collection '%s dphi(r)'; Group '%s'; DataSet '%s dphi(r)'; Source XYData '%s//DeltaPhiR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);

				// Inversion (delta g(r), FT etc.)
				blockData.sprintf("Collection '%s dg(r)'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet '%s Delta g(r)'; Source XYData '%s//Inversion//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);

				// Delta g(r)
				blockData.sprintf("Collection '%s dBound(r)'; Group '%s'; LineStyle 1.0 'Dots'; DataSet '%s Delta Bound g(r)'; Source XYData '%s//DeltaGRBond//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);
			}
		}
	}

	// Add phi magnitude data
	blockData.sprintf("Collection 'PhiMag'; Group 'PhiMag; DataSet 'PhiMag'; Source XYData '%s//PhiMag'; EndDataSet; EndCollection", module_->uniqueName());
	phiMagGraph_->addCollectionFromBlock(blockData);
}


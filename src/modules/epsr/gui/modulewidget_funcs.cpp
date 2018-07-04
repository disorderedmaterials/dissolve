/*
	*** EPSR Module Widget - Functions
	*** src/modules/epsr/gui/modulewidget_funcs.cpp
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

#include "modules/epsr/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
EPSRModuleWidget::EPSRModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((EPSRModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = true;

	ViewPane* viewPane;

	// F(Q) Graph

	FQGraph_ = ui.FQPlotWidget;

	// Start a new, empty session
	FQGraph_->startNewSession(true);
	viewPane = FQGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "F(\\it{Q})");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// F(Q) Fitting Graph

	FQFitGraph_ = ui.FQFitPlotWidget;

	// Start a new, empty session
	FQFitGraph_->startNewSession(true);
	viewPane = FQFitGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "\\sym{Delta}F(\\it{Q})");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Partial S(Q) Graph
	
	SQGraph_ = ui.SQPlotWidget;

	// Start a new, empty session
	SQGraph_->startNewSession(true);
	viewPane = SQGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(\\it{Q})");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// g(r) Graph
	
	GRGraph_ = ui.GRPlotWidget;

	// Start a new, empty session
	GRGraph_->startNewSession(true);
	viewPane = GRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// F(r) Graph
	
	FRGraph_ = ui.GRPlotWidget;

	// Start a new, empty session
	FRGraph_->startNewSession(true);
	viewPane = FRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "F(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Phi(r) (Empirical Potentials) Graph

	phiRGraph_ = ui.PhiRPlotWidget;

	// Start a new, empty session
	phiRGraph_->startNewSession(true);
	viewPane = phiRGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "\\sym{phi}(\\it{r})");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// phi(r) Magnitude Graph

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

EPSRModuleWidget::~EPSRModuleWidget()
{
}

// Update controls within widget
void EPSRModuleWidget::updateControls()
{
	refreshing_ = true;

	// Ensure that any displayed data are up-to-date
	FQGraph_->refreshReferencedDataSets();
	FQFitGraph_->refreshReferencedDataSets();
	SQGraph_->refreshReferencedDataSets();
	GRGraph_->refreshReferencedDataSets();
	FRGraph_->refreshReferencedDataSets();
	phiRGraph_->refreshReferencedDataSets();
	phiMagGraph_->refreshReferencedDataSets();
	errorsGraph_->refreshReferencedDataSets();

	FQGraph_->updateDisplay();
	FQFitGraph_->updateDisplay();
	SQGraph_->updateDisplay();
	GRGraph_->updateDisplay();
	FRGraph_->updateDisplay();
	phiRGraph_->updateDisplay();
	phiMagGraph_->updateDisplay();
	errorsGraph_->updateDisplay();

	refreshing_ = false;
}

// Disable sensitive controls within widget, ready for main code to run
void EPSRModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void EPSRModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool EPSRModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!FQGraph_->writeSession(parser)) return false;
	if (!FQFitGraph_->writeSession(parser)) return false;
	if (!SQGraph_->writeSession(parser)) return false;
	if (!GRGraph_->writeSession(parser)) return false;
	if (!FRGraph_->writeSession(parser)) return false;
	if (!phiRGraph_->writeSession(parser)) return false;
	if (!phiMagGraph_->writeSession(parser)) return false;
	if (!errorsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool EPSRModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!FQGraph_->readSession(parser)) return false;
	if (!FQFitGraph_->readSession(parser)) return false;
	if (!SQGraph_->readSession(parser)) return false;
	if (!GRGraph_->readSession(parser)) return false;
	if (!FRGraph_->readSession(parser)) return false;
	if (!phiRGraph_->readSession(parser)) return false;
	if (!phiMagGraph_->readSession(parser)) return false;
	if (!errorsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void EPSRModuleWidget::setGraphDataTargets(EPSRModule* module)
{
	if (!module) return;

	const int nTypes = dissolve_.atomTypeList().nItems();
	int n, m;
	CharString blockData;

	// Add reference data & calculated data to the FQGraph_, and percentage errors to the errorsGraph_
	RefListIterator<Module,bool> targetIterator(module->targets());
	while (Module* targetModule = targetIterator.iterate())
	{
		// Reference data
		blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source XYData '%s//ReferenceData'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
		FQGraph_->addCollectionFromBlock(blockData);

		// Calculated data from associated module
		if (DissolveSys::sameString(targetModule->name(), "NeutronSQ"))
		{
			blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source XYData '%s//WeightedSQ//Total'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
			FQGraph_->addCollectionFromBlock(blockData);

			blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle  1.0 'Dots'; DataSet '%s Error'; Source XYData '%s//Difference//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
			FQGraph_->addCollectionFromBlock(blockData);

			blockData.sprintf("Collection '%s Calc'; Group '%s'; DataSet '%s Error'; Source XYData '%s//Error//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
			errorsGraph_->addCollectionFromBlock(blockData);
		}

		// Delta F(Q) and fit
		blockData.sprintf("Collection '%s Delta'; Group '%s'; DataSet '%s Delta'; Source XYData '%s//DeltaFQ//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
		FQFitGraph_->addCollectionFromBlock(blockData);

		blockData.sprintf("Collection '%s Fit'; Group '%s'; LineStyle  1.0 'Quarter Dash'; DataSet '%s Fit'; Source XYData '%s//DeltaFQFit//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
		FQFitGraph_->addCollectionFromBlock(blockData);
	}

	// Loop over groups
	for (ModuleGroup* group = module_->targetGroups().first(); group != NULL; group = group->next)
	{
		// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the SQGraph_
		n = 0;
		for (AtomType* at1 = dissolve_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
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
				SQGraph_->addCollectionFromBlock(blockData);

				// Calculated / summed partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source XYData '%s//UnweightedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				SQGraph_->addCollectionFromBlock(blockData);

				// Deltas
				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle 1.0 'Dots'; DataSet 'Delta %s'; Source XYData '%s//DeltaSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				SQGraph_->addCollectionFromBlock(blockData);

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source XYData '%s//GeneratedGR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				GRGraph_->addCollectionFromBlock(blockData);

				// Calculated partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source XYData '%s//UnweightedGR//%s//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				GRGraph_->addCollectionFromBlock(blockData);
			}
		}
	}

	n = 0;
	for (AtomType* at1 = dissolve_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			// Generated potential
			blockData.sprintf("Collection '%s phi(r)'; Group '%s'; DataSet '%s phi(r)'; Source XYData 'PairPotential//%s-%s//Additional'; EndDataSet; EndCollection", id.get(), id.get(), id.get(),  at1->name(), at2->name());
			phiRGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Add phi magnitude data
	blockData.sprintf("Collection 'PhiMag'; Group 'PhiMag; DataSet 'PhiMag'; Source XYData '%s//PhiMag'; EndDataSet; EndCollection", module_->uniqueName());
	phiMagGraph_->addCollectionFromBlock(blockData);
}


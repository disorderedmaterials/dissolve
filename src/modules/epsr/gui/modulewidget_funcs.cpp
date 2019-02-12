/*
	*** EPSR Module Widget - Functions
	*** src/modules/epsr/gui/modulewidget_funcs.cpp
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

#include "modules/epsr/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "module/group.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
EPSRModuleWidget::EPSRModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((EPSRModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = true;

	// F(Q) Graph

	FQGraph_ = ui.FQPlotWidget->dataViewer();

	// Start a new, empty session
	FQGraph_->startNewSession(true);
	FQGraph_->view().setViewType(View::FlatXYView);
	FQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	FQGraph_->view().axes().setMax(0, 10.0);
	FQGraph_->view().axes().setTitle(1, "F(\\it{Q})");
	FQGraph_->view().axes().setMin(1, -1.0);
	FQGraph_->view().axes().setMax(1, 1.0);
	FQGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	FQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// F(Q) Fitting Graph

	FQFitGraph_ = ui.FQFitPlotWidget->dataViewer();

	// Start a new, empty session
	FQFitGraph_->startNewSession(true);
	FQFitGraph_->view().setViewType(View::FlatXYView);
	FQFitGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	FQFitGraph_->view().axes().setMax(0, 10.0);
	FQFitGraph_->view().axes().setTitle(1, "\\sym{Delta}F(\\it{Q})");
	FQFitGraph_->view().axes().setMin(1, -1.0);
	FQFitGraph_->view().axes().setMax(1, 1.0);
	FQFitGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	FQFitGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial S(Q) Graph
	
	SQGraph_ = ui.SQPlotWidget->dataViewer();

	// Start a new, empty session
	SQGraph_->startNewSession(true);
	SQGraph_->view().setViewType(View::FlatXYView);
	SQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	SQGraph_->view().axes().setMax(0, 10.0);
	SQGraph_->view().axes().setTitle(1, "S(\\it{Q})");
	SQGraph_->view().axes().setMin(1, -1.0);
	SQGraph_->view().axes().setMax(1, 1.0);
	SQGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	SQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// g(r) Graph
	
	GRGraph_ = ui.GRPlotWidget->dataViewer();

	// Start a new, empty session
	GRGraph_->startNewSession(true);
	GRGraph_->view().setViewType(View::FlatXYView);
	GRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	GRGraph_->view().axes().setMax(0, 10.0);
	GRGraph_->view().axes().setTitle(1, "g(r)");
	GRGraph_->view().axes().setMin(1, -1.0);
	GRGraph_->view().axes().setMax(1, 1.0);
	GRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	GRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// F(r) Graph
	
	FRGraph_ = ui.FRPlotWidget->dataViewer();

	// Start a new, empty session
	FRGraph_->startNewSession(true);
	FRGraph_->view().setViewType(View::FlatXYView);
	FRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	FRGraph_->view().axes().setMax(0, 10.0);
	FRGraph_->view().axes().setTitle(1, "F(r)");
	FRGraph_->view().axes().setMin(1, -1.0);
	FRGraph_->view().axes().setMax(1, 1.0);
	FRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	FRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Phi(r) (Empirical Potentials) Graph

	phiRGraph_ = ui.PhiRPlotWidget->dataViewer();

	// Start a new, empty session
	phiRGraph_->startNewSession(true);
	phiRGraph_->view().setViewType(View::FlatXYView);
	phiRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	phiRGraph_->view().axes().setMax(0, 10.0);
	phiRGraph_->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
	phiRGraph_->view().axes().setMin(1, -1.0);
	phiRGraph_->view().axes().setMax(1, 1.0);
	phiRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	phiRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// phi(r) Magnitude Graph

	phiMagGraph_ = ui.PhiMagPlotWidget->dataViewer();

	// Start a new, empty session
	phiMagGraph_->startNewSession(true);
	phiMagGraph_->view().setViewType(View::FlatXYView);
	phiMagGraph_->view().axes().setTitle(0, "Iteration");
	phiMagGraph_->view().axes().numberFormat(0).setNDecimals(0);
	phiMagGraph_->view().axes().setMax(0, 10.0);
	phiMagGraph_->view().axes().setTitle(1, "\\sym{Delta}\\sym{phi}(\\it{r}), kJ mol\\sup{-1} \\sum{angstrom}\\sup{-1}");
	phiMagGraph_->view().axes().setMin(1, 0.0);
	phiMagGraph_->view().axes().setMax(1, 1.0);
	phiMagGraph_->view().setAutoFollowType(View::XFollow);

	// R-Factor Graph

	rFactorGraph_ = ui.RFactorPlotWidget->dataViewer();

	// Start a new, empty session
	rFactorGraph_->startNewSession(true);
	rFactorGraph_->view().setViewType(View::FlatXYView);
	rFactorGraph_->view().axes().setTitle(0, "Iteration");
	rFactorGraph_->view().axes().numberFormat(0).setNDecimals(0);
	rFactorGraph_->view().axes().setMax(0, 10.0);
	rFactorGraph_->view().axes().setTitle(1, "R-Factor");
	rFactorGraph_->view().axes().setMin(1, 0.0);
	rFactorGraph_->view().axes().setMax(1, 0.5);
	rFactorGraph_->view().setAutoFollowType(View::AllAutoFollow);

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
	rFactorGraph_->refreshReferencedDataSets();

	FQGraph_->postRedisplay();
	FQFitGraph_->postRedisplay();
	SQGraph_->postRedisplay();
	GRGraph_->postRedisplay();
	FRGraph_->postRedisplay();
	phiRGraph_->postRedisplay();
	phiMagGraph_->postRedisplay();
	rFactorGraph_->postRedisplay();

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
	// Write DataViewer sessions
	if (!FQGraph_->writeSession(parser)) return false;
	if (!FQFitGraph_->writeSession(parser)) return false;
	if (!SQGraph_->writeSession(parser)) return false;
	if (!GRGraph_->writeSession(parser)) return false;
	if (!FRGraph_->writeSession(parser)) return false;
	if (!phiRGraph_->writeSession(parser)) return false;
	if (!phiMagGraph_->writeSession(parser)) return false;
	if (!rFactorGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool EPSRModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!FQGraph_->readSession(parser)) return false;
	if (!FQFitGraph_->readSession(parser)) return false;
	if (!SQGraph_->readSession(parser)) return false;
	if (!GRGraph_->readSession(parser)) return false;
	if (!FRGraph_->readSession(parser)) return false;
	if (!phiRGraph_->readSession(parser)) return false;
	if (!phiMagGraph_->readSession(parser)) return false;
	if (!rFactorGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void EPSRModuleWidget::setGraphDataTargets(EPSRModule* module)
{
	if (!module) return;

	int n, m;
	CharString blockData;

	// Add total R-Factor before any dataset R-Factors
	blockData.sprintf("Collection 'Total'; Group 'Total'; DataSet 'Total'; Source Data1D '%s//RFactor'; EndDataSet; EndCollection", module->uniqueName());
	rFactorGraph_->addCollectionFromBlock(blockData);

	// Add reference data & calculated data to the FQGraph_, and percentage errors to the rFactorGraph_
	RefListIterator<Module,bool> targetIterator(module->allTargets());
	while (Module* targetModule = targetIterator.iterate())
	{
		// Reference data
		blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source Data1D '%s//ReferenceData'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
		FQGraph_->addCollectionFromBlock(blockData);

		// Calculated data from associated module
		if (DissolveSys::sameString(targetModule->type(), "NeutronSQ"))
		{
			// F(Q)
			blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source Data1D '%s//WeightedSQ//Total'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
			FQGraph_->addCollectionFromBlock(blockData);

			// F(Q) diff w.r.t. reference
			blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle  1.0 'Dots'; DataSet '%s Error'; Source Data1D '%s//Difference//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
			FQGraph_->addCollectionFromBlock(blockData);

			// R-Factor between F(Q) and reference
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet '%s'; Source Data1D '%s//RFactor//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
			rFactorGraph_->addCollectionFromBlock(blockData);

			// Reference F(r) (from direct FT of input data)
			blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source Data1D '%s//ReferenceDataFT'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
			FRGraph_->addCollectionFromBlock(blockData);

			// Simulated F(r) (from FT of the calculated F(Q))
			blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source Data1D '%s//SimulatedFR//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
			FRGraph_->addCollectionFromBlock(blockData);
		}

		// Delta F(Q) and fit
		blockData.sprintf("Collection '%s Delta'; Group '%s'; DataSet '%s Delta'; Source Data1D '%s//DeltaFQ//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
		FQFitGraph_->addCollectionFromBlock(blockData);

		blockData.sprintf("Collection '%s Fit'; Group '%s'; LineStyle  1.0 'Quarter Dash'; DataSet '%s Fit'; Source Data1D '%s//DeltaFQFit//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
		FQFitGraph_->addCollectionFromBlock(blockData);

	}

	// Loop over groups
	ListIterator<ModuleGroup> groupIterator(module_->groupedTargets().groups());
	while (ModuleGroup* group = groupIterator.iterate())
	{
		// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the SQGraph_
		n = 0;
		for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++n)
		{
			m = n;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
			{
				CharString id("%s-%s/%s", at1->name(), at2->name(), group->name());

				/*
				 * Partial Structure Factors
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source Data1D '%s//GeneratedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				SQGraph_->addCollectionFromBlock(blockData);

				// Calculated / summed partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				SQGraph_->addCollectionFromBlock(blockData);

				// Deltas
				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle 1.0 'Dots'; DataSet 'Delta %s'; Source Data1D '%s//DeltaSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				SQGraph_->addCollectionFromBlock(blockData);

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source Data1D '%s//GeneratedGR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				GRGraph_->addCollectionFromBlock(blockData);

				// Calculated partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedGR//%s//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				GRGraph_->addCollectionFromBlock(blockData);
			}
		}
	}

	n = 0;
	for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			// Generated potential
			blockData.sprintf("Collection '%s phi(r)'; Group '%s'; DataSet '%s phi(r)'; Source Data1D 'PairPotential//%s-%s//Additional'; EndDataSet; EndCollection", id.get(), id.get(), id.get(),  at1->name(), at2->name());
			phiRGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Add phi magnitude data
	blockData.sprintf("Collection 'EPMag'; Group 'EPMag'; DataSet 'EPMag'; Source Data1D '%s//EPMag'; EndDataSet; EndCollection", module_->uniqueName());
	phiMagGraph_->addCollectionFromBlock(blockData);
}


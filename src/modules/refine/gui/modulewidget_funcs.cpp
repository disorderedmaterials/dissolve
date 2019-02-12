/*
	*** Refine Module Widget - Functions
	*** src/modules/refine/gui/modulewidget_funcs.cpp
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

#include "modules/refine/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include "module/group.h"
#include "modules/refine/refine.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
RefineModuleWidget::RefineModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((RefineModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = true;

	ViewPane* viewPane;

	// Data Graph

	dataGraph_ = ui.DataPlotWidget->dataViewer();

	// Start a new, empty session
	dataGraph_->startNewSession(true);
	dataGraph_->view().setViewType(View::FlatXYView);
	dataGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	dataGraph_->view().axes().setMax(0, 10.0);
	dataGraph_->view().axes().setTitle(1, "S(Q)");
	dataGraph_->view().axes().setMin(1, -1.0);
	dataGraph_->view().axes().setMax(1, 1.0);
	dataGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
	dataGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial S(Q) Graph
	
	partialSQGraph_ = ui.PartialSQPlotWidget->dataViewer();

	// Start a new, empty session
	partialSQGraph_->startNewSession(true);
	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial g(r) Graph
	
	partialGRGraph_ = ui.PartialGRPlotWidget->dataViewer();

	// Start a new, empty session
	partialGRGraph_->startNewSession(true);
	partialGRGraph_->view().setViewType(View::FlatXYView);
	partialGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	partialGRGraph_->view().axes().setMax(0, 10.0);
	partialGRGraph_->view().axes().setTitle(1, "g(r)");
	partialGRGraph_->view().axes().setMin(1, -1.0);
	partialGRGraph_->view().axes().setMax(1, 1.0);
	partialGRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Delta phi(r) Graph

	deltaPhiRGraph_ = ui.DeltaPhiRPlotWidget->dataViewer();

	// Start a new, empty session
	deltaPhiRGraph_->startNewSession(true);
	deltaPhiRGraph_->view().setViewType(View::FlatXYView);
	deltaPhiRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	deltaPhiRGraph_->view().axes().setMax(0, 10.0);
	deltaPhiRGraph_->view().axes().setTitle(1, "\\sym{Delta}g(r), \\sym{Delta}\\sym{phi}(\\it{r})");
	deltaPhiRGraph_->view().axes().setMin(1, -1.0);
	deltaPhiRGraph_->view().axes().setMax(1, 1.0);
	deltaPhiRGraph_->view().collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	deltaPhiRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Delta phi(r) Graph

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

	// Errors Graph

	errorsGraph_ = ui.ErrorsPlotWidget->dataViewer();

	// Start a new, empty session
	errorsGraph_->startNewSession(true);
	errorsGraph_->view().setViewType(View::FlatXYView);
	errorsGraph_->view().axes().setTitle(0, "Iteration");
	errorsGraph_->view().axes().numberFormat(0).setNDecimals(0);
	errorsGraph_->view().axes().setMax(0, 10.0);
	errorsGraph_->view().axes().setTitle(1, "%Error");
	errorsGraph_->view().axes().setMin(1, 0.0);
	errorsGraph_->view().axes().setMax(1, 100.0);
	errorsGraph_->view().setAutoFollowType(View::XFollow);

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
	ListIterator<PairPotential> ppIterator(dissolve_.pairPotentials());
	while (PairPotential* pp = ppIterator.iterate()) phiLevel += Integrator::absTrapezoid(pp->uAdditional());
	ui.PhiLevelSpin->setValue(phiLevel);

	// Ensure that any displayed data are up-to-date
	dataGraph_->refreshReferencedDataSets();
	partialSQGraph_->refreshReferencedDataSets();
	partialGRGraph_->refreshReferencedDataSets();
	deltaPhiRGraph_->refreshReferencedDataSets();
	phiMagGraph_->refreshReferencedDataSets();
	errorsGraph_->refreshReferencedDataSets();

	dataGraph_->postRedisplay();
	partialSQGraph_->postRedisplay();
	partialGRGraph_->postRedisplay();
	deltaPhiRGraph_->postRedisplay();
	phiMagGraph_->postRedisplay();
	errorsGraph_->postRedisplay();

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
	// Write DataViewer sessions
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
	// Read DataViewer sessions
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

	int n, m;
	CharString blockData;

	// Loop over groups
	ListIterator<ModuleGroup> groupIterator(module_->groupedTargets().groups());
	while (ModuleGroup* group = groupIterator.iterate())
	{
		// Add reference data & calculated data to the dataGraph_, and percentage errors to the errorsGraph_
		RefListIterator<Module,bool> targetIterator(group->modules());
		while (Module* targetModule = targetIterator.iterate())
		{
			// Reference data
			blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source Data1D '%s//ReferenceData'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
			dataGraph_->addCollectionFromBlock(blockData);

			// Calculated data from associated module
			if (DissolveSys::sameString(targetModule->type(), "NeutronSQ"))
			{
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source Data1D '%s//WeightedSQ//Total'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName());
				dataGraph_->addCollectionFromBlock(blockData);

				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle  1.0 'Dots'; DataSet '%s Error'; Source Data1D '%s//Difference//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
				dataGraph_->addCollectionFromBlock(blockData);

				blockData.sprintf("Collection '%s Calc'; Group '%s'; DataSet '%s Error'; Source Data1D '%s//Error//%s'; EndDataSet; EndCollection", targetModule->uniqueName(), targetModule->uniqueName(), targetModule->uniqueName(), module->uniqueName(), targetModule->uniqueName());
				errorsGraph_->addCollectionFromBlock(blockData);
			}
		}

		// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the partialSQGraph_
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
				partialSQGraph_->addCollectionFromBlock(blockData);

				// Calculated / summed partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialSQGraph_->addCollectionFromBlock(blockData);

				// Deltas
				blockData.sprintf("Collection '%s Diff'; Group '%s'; LineStyle 1.0 'Dots'; DataSet 'Delta %s'; Source Data1D '%s//DeltaSQ//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialSQGraph_->addCollectionFromBlock(blockData);

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				blockData.sprintf("Collection '%s Exp'; Group '%s'; DataSet 'Experimental %s'; Source Data1D '%s//GeneratedGR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialGRGraph_->addCollectionFromBlock(blockData);

				// Calculated partial
				blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedGR//%s//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				partialGRGraph_->addCollectionFromBlock(blockData);

				/*
				 * Phi(r)
				 */

				// Generated potential
				blockData.sprintf("Collection '%s dphi(r)'; Group '%s'; DataSet '%s dphi(r)'; Source Data1D '%s//DeltaPhiR//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);

				// Inversion (delta g(r), FT etc.)
				blockData.sprintf("Collection '%s dg(r)'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet '%s Delta g(r)'; Source Data1D '%s//Inversion//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);

				// Delta g(r)
				blockData.sprintf("Collection '%s dBound(r)'; Group '%s'; LineStyle 1.0 'Dots'; DataSet '%s Delta Bound g(r)'; Source Data1D '%s//DeltaGRBond//%s//%s-%s'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), module_->uniqueName(), group->name(), at1->name(), at2->name());
				deltaPhiRGraph_->addCollectionFromBlock(blockData);
			}
		}
	}

	// Add phi magnitude data
	blockData.sprintf("Collection 'PhiMag'; Group 'PhiMag; DataSet 'PhiMag'; Source Data1D '%s//PhiMag'; EndDataSet; EndCollection", module_->uniqueName());
	phiMagGraph_->addCollectionFromBlock(blockData);
}


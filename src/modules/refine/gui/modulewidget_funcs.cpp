/*
	*** Refine Module Widget - Functions
	*** src/modules/refine/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2020

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
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include "module/group.h"
#include "modules/refine/refine.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
RefineModuleWidget::RefineModuleWidget(QWidget* parent, RefineModule* module, Dissolve& dissolve) : ModuleWidget(parent), module_(module), dissolve_(dissolve)
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = true;

	NumberFormat numberFormat;

	// Data Graph

	dataGraph_ = ui_.DataPlotWidget->dataViewer();

	// Start a new, empty session
	dataGraph_->view().setViewType(View::FlatXYView);
	dataGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	dataGraph_->view().axes().setMax(0, 10.0);
	dataGraph_->view().axes().setTitle(1, "S(Q)");
	dataGraph_->view().axes().setMin(1, -1.0);
	dataGraph_->view().axes().setMax(1, 1.0);
	dataGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
	dataGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial S(Q) Graph
	
	partialSQGraph_ = ui_.PartialSQPlotWidget->dataViewer();

	// Start a new, empty session
	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial g(r) Graph
	
	partialGRGraph_ = ui_.PartialGRPlotWidget->dataViewer();

	// Start a new, empty session
	partialGRGraph_->view().setViewType(View::FlatXYView);
	partialGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	partialGRGraph_->view().axes().setMax(0, 10.0);
	partialGRGraph_->view().axes().setTitle(1, "g(r)");
	partialGRGraph_->view().axes().setMin(1, -1.0);
	partialGRGraph_->view().axes().setMax(1, 1.0);
	partialGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Delta phi(r) Graph

	deltaPhiRGraph_ = ui_.DeltaPhiRPlotWidget->dataViewer();

	// Start a new, empty session
	deltaPhiRGraph_->view().setViewType(View::FlatXYView);
	deltaPhiRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	deltaPhiRGraph_->view().axes().setMax(0, 10.0);
	deltaPhiRGraph_->view().axes().setTitle(1, "\\sym{Delta}g(r), \\sym{Delta}\\sym{phi}(\\it{r})");
	deltaPhiRGraph_->view().axes().setMin(1, -1.0);
	deltaPhiRGraph_->view().axes().setMax(1, 1.0);
	deltaPhiRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	deltaPhiRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Phi(r) Magnitude Graph

	phiMagGraph_ = ui_.PhiMagPlotWidget->dataViewer();

	// Start a new, empty session
	phiMagGraph_->view().setViewType(View::FlatXYView);
	phiMagGraph_->view().axes().setTitle(0, "Iteration");
	numberFormat = phiMagGraph_->view().axes().numberFormat(0);
	numberFormat.setNDecimals(0);
	phiMagGraph_->view().axes().setNumberFormat(0, numberFormat);
	phiMagGraph_->view().axes().setMax(0, 10.0);
	phiMagGraph_->view().axes().setTitle(1, "\\sym{Delta}\\sym{phi}(\\it{r}), kJ mol\\sup{-1} \\sum{angstrom}\\sup{-1}");
	phiMagGraph_->view().axes().setMin(1, 0.0);
	phiMagGraph_->view().axes().setMax(1, 1.0);
	phiMagGraph_->view().setAutoFollowType(View::XAutoFollow);

	// Errors Graph

	errorsGraph_ = ui_.ErrorsPlotWidget->dataViewer();

	// Start a new, empty session
	errorsGraph_->view().setViewType(View::FlatXYView);
	errorsGraph_->view().axes().setTitle(0, "Iteration");
	numberFormat = errorsGraph_->view().axes().numberFormat(0);
	numberFormat.setNDecimals(0);
	errorsGraph_->view().axes().setNumberFormat(0, numberFormat);
	errorsGraph_->view().axes().setMax(0, 10.0);
	errorsGraph_->view().axes().setTitle(1, "%Error");
	errorsGraph_->view().axes().setMin(1, 0.0);
	errorsGraph_->view().axes().setMax(1, 100.0);
	errorsGraph_->view().setAutoFollowType(View::XAutoFollow);

	setGraphDataTargets(module_);

	updateControls();

	refreshing_ = false;
}

RefineModuleWidget::~RefineModuleWidget()
{
}

/*
 * UI
 */

// Update controls within widget
void RefineModuleWidget::updateControls(int flags)
{
	refreshing_ = true;

	// Set controls on Overview page
	double phiLevel = 0.0;
	ListIterator<PairPotential> ppIterator(dissolve_.pairPotentials());
	while (PairPotential* pp = ppIterator.iterate()) phiLevel += Integrator::absTrapezoid(pp->uAdditional());
	ui_.PhiLevelSpin->setValue(phiLevel);

	ui_.DataPlotWidget->updateToolbar();
	ui_.PartialSQPlotWidget->updateToolbar();	
	ui_.PartialGRPlotWidget->updateToolbar();
	ui_.DeltaPhiRPlotWidget->updateToolbar();
	ui_.PhiMagPlotWidget->updateToolbar();
	ui_.ErrorsPlotWidget->updateToolbar();

	dataGraph_->postRedisplay();
	partialSQGraph_->postRedisplay();
	partialGRGraph_->postRedisplay();
	deltaPhiRGraph_->postRedisplay();
	phiMagGraph_->postRedisplay();
	errorsGraph_->postRedisplay();

	refreshing_ = false;
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool RefineModuleWidget::writeState(LineParser& parser) const
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
		for (Module* targetModule : group->modules())
		{
			// Reference data
			Renderable* refData = dataGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceData", targetModule->uniqueName()), CharString("ExpFQ//%s", targetModule->uniqueName()), CharString("%s Exp", targetModule->uniqueName()));
			dataGraph_->addRenderableToGroup(refData, targetModule->uniqueName());

			// Calculated data from associated module
			if (DissolveSys::sameString(targetModule->type(), "NeutronSQ"))
			{
				// Calculated F(Q)
				Renderable* calcFQ = dataGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//Total", targetModule->uniqueName()), CharString("CalcFQ//%s", targetModule->uniqueName()), CharString("%s Calc", targetModule->uniqueName()));
				dataGraph_->addRenderableToGroup(calcFQ, targetModule->uniqueName());

				// F(Q) diff w.r.t. reference
				Renderable* diffFQ = dataGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//Difference//%s", module->uniqueName(), targetModule->uniqueName()), CharString("DiffFQ//%s//%s", module->uniqueName(), targetModule->uniqueName()), CharString("%s Diff", targetModule->uniqueName()));
				diffFQ->lineStyle().setStipple(LineStipple::DotStipple);
				dataGraph_->addRenderableToGroup(diffFQ, targetModule->uniqueName());

				// Error
				Renderable* error = errorsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//Error//%s", module->uniqueName(), targetModule->uniqueName()), CharString("Error//%s//%s", module->uniqueName(), targetModule->uniqueName()), targetModule->uniqueName());
				dataGraph_->addRenderableToGroup(error, targetModule->uniqueName());
			}
		}

		// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the partialSQGraph_
		n = 0;
		for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++n)
		{
			m = n;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++m)
			{
				CharString id("%s-%s/%s", at1->name(), at2->name(), group->name());

				/*
				 * Partial Structure Factors
				 */

				// Unweighted estimated partial
				Renderable* estSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//EstimatedSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("ExpSQ//%s", id.get()), CharString("%s Estimated", id.get()));
				partialSQGraph_->addRenderableToGroup(estSQ, id.get());

				// Calculated / summed partial
				Renderable* calcSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("CalcSQ//%s", id.get()), CharString("%s Calc", id.get()));
				calcSQ->lineStyle().setStipple(LineStipple::QuarterDashStipple);
				partialSQGraph_->addRenderableToGroup(calcSQ, id.get());

				// Deltas
				Renderable* deltaSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("DeltaSQ//%s", id.get()), CharString("%s Delta", id.get()));
				deltaSQ->lineStyle().setStipple(LineStipple::DotStipple);
				partialSQGraph_->addRenderableToGroup(deltaSQ, id.get());

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				Renderable* estGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//EstimatedGR//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("ExpGR//%s", id.get()), CharString("%s Estimated", id.get()));
				partialGRGraph_->addRenderableToGroup(estGR, id.get());

				// Calculated / summed partial
				Renderable* calcGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s//%s-%s//Full", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("CalcGR//%s", id.get()), CharString("%s Calc", id.get()));
				calcGR->lineStyle().setStipple(LineStipple::QuarterDashStipple);
				partialGRGraph_->addRenderableToGroup(calcGR, id.get());

				/*
				 * Phi(r)
				 */

				// Generated potential
				Renderable* deltaPhiR = deltaPhiRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaPhiR//%s//%s-%s//Full", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("DeltaPhiR//%s//%s-%s", group->name(), at1->name(), at2->name()), CharString("%s dphi(r)", id.get()));

				// Inversion (delta g(r), FT etc.)
				Renderable* inversion = deltaPhiRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//Inversion//%s//%s-%s//Full", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("Inversion//%s//%s-%s", group->name(), at1->name(), at2->name()), CharString("%s dFull(r)", id.get()));
				inversion->lineStyle().setStipple(LineStipple::QuarterDashStipple);

				// Delta g(r)
				Renderable* deltaBond = deltaPhiRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaGRBond//%s//%s-%s//Full", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("DeltaGR//%s//%s-%s", group->name(), at1->name(), at2->name()), CharString("%s dBound(r)", id.get()));
				deltaBond->lineStyle().setStipple(LineStipple::DotStipple);
			}
		}
	}

	// Add phi magnitude data
	phiMagGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//PhiMag", module_->uniqueName()), "PhiMag", "PhiMag");
}


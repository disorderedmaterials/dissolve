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
#include "genericitems/listhelper.h"

// Constructor
EPSRModuleWidget::EPSRModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<EPSRModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = true;

	// F(Q) Graph

	FQGraph_ = ui_.FQPlotWidget->dataViewer();

	// Start a new, empty session
	FQGraph_->view().setViewType(View::FlatXYView);
	FQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	FQGraph_->view().axes().setMax(0, 10.0);
	FQGraph_->view().axes().setTitle(1, "F(\\it{Q})");
	FQGraph_->view().axes().setMin(1, -1.0);
	FQGraph_->view().axes().setMax(1, 1.0);
	FQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
	FQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// F(Q) Fitting Graph

	FQFitGraph_ = ui_.FQFitPlotWidget->dataViewer();

	// Start a new, empty session
	FQFitGraph_->view().setViewType(View::FlatXYView);
	FQFitGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	FQFitGraph_->view().axes().setMax(0, 10.0);
	FQFitGraph_->view().axes().setTitle(1, "\\sym{Delta}F(\\it{Q})");
	FQFitGraph_->view().axes().setMin(1, -1.0);
	FQFitGraph_->view().axes().setMax(1, 1.0);
	FQFitGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
	FQFitGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Partial S(Q) Graph
	
	SQGraph_ = ui_.SQPlotWidget->dataViewer();

	// Start a new, empty session
	SQGraph_->view().setViewType(View::FlatXYView);
	SQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	SQGraph_->view().axes().setMax(0, 10.0);
	SQGraph_->view().axes().setTitle(1, "S(\\it{Q})");
	SQGraph_->view().axes().setMin(1, -1.0);
	SQGraph_->view().axes().setMax(1, 1.0);
	SQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	SQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// g(r) Graph
	
	GRGraph_ = ui_.GRPlotWidget->dataViewer();

	// Start a new, empty session
	GRGraph_->view().setViewType(View::FlatXYView);
	GRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	GRGraph_->view().axes().setMax(0, 10.0);
	GRGraph_->view().axes().setTitle(1, "g(r)");
	GRGraph_->view().axes().setMin(1, -1.0);
	GRGraph_->view().axes().setMax(1, 1.0);
	GRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	GRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// F(r) Graph
	
	FRGraph_ = ui_.FRPlotWidget->dataViewer();

	// Start a new, empty session
	FRGraph_->view().setViewType(View::FlatXYView);
	FRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	FRGraph_->view().axes().setMax(0, 10.0);
	FRGraph_->view().axes().setTitle(1, "F(r)");
	FRGraph_->view().axes().setMin(1, -1.0);
	FRGraph_->view().axes().setMax(1, 1.0);
	FRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	FRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Phi(r) (Empirical Potentials) Graph

	phiRGraph_ = ui_.PhiRPlotWidget->dataViewer();

	// Start a new, empty session
	phiRGraph_->view().setViewType(View::FlatXYView);
	phiRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	phiRGraph_->view().axes().setMax(0, 10.0);
	phiRGraph_->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
	phiRGraph_->view().axes().setMin(1, -1.0);
	phiRGraph_->view().axes().setMax(1, 1.0);
	phiRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	phiRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// phi(r) Magnitude Graph

	phiMagGraph_ = ui_.PhiMagPlotWidget->dataViewer();

	// Start a new, empty session
	phiMagGraph_->view().setViewType(View::FlatXYView);
	phiMagGraph_->view().axes().setTitle(0, "Iteration");
	phiMagGraph_->view().axes().numberFormat(0).setType(NumberFormat::IntegerFormat);
	phiMagGraph_->view().axes().setMax(0, 10.0);
	phiMagGraph_->view().axes().setTitle(1, "\\sym{Delta}\\sym{phi}(\\it{r}), kJ mol\\sup{-1} \\sum{angstrom}\\sup{-1}");
	phiMagGraph_->view().axes().setMin(1, 0.0);
	phiMagGraph_->view().axes().setMax(1, 1.0);
	phiMagGraph_->view().setAutoFollowType(View::XAutoFollow);

	// R-Factor Graph

	rFactorGraph_ = ui_.RFactorPlotWidget->dataViewer();

	// Start a new, empty session
	rFactorGraph_->view().setViewType(View::FlatXYView);
	rFactorGraph_->view().axes().setTitle(0, "Iteration");
	rFactorGraph_->view().axes().numberFormat(0).setType(NumberFormat::IntegerFormat);
	rFactorGraph_->view().axes().setMax(0, 10.0);
	rFactorGraph_->view().axes().setTitle(1, "R-Factor");
	rFactorGraph_->view().axes().numberFormat(1).setType(NumberFormat::ScientificFormat);
	rFactorGraph_->view().axes().setMin(1, 0.0);
	rFactorGraph_->view().axes().setMax(1, 0.5);
	rFactorGraph_->view().setAutoFollowType(View::AllAutoFollow);

	setGraphDataTargets(module_);

	// Debug Tab - EP Functions Graph

	DataViewer* epView = ui_.DebugEPFunctionsPlotWidget->dataViewer();

	// Start a new, empty session
	epView->view().setViewType(View::FlatXYView);
	epView->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	epView->view().axes().setMax(0, 10.0);
	epView->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
	epView->view().axes().setMin(1, -1.0);
	epView->view().axes().setMax(1, 1.0);
	epView->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

	updateControls();

	refreshing_ = false;
}

EPSRModuleWidget::~EPSRModuleWidget()
{
}

// Update controls within widget
void EPSRModuleWidget::updateControls()
{
	refreshing_ = true;

	ui_.FQPlotWidget->updateToolbar();
	ui_.FQFitPlotWidget->updateToolbar();
	ui_.SQPlotWidget->updateToolbar();
	ui_.GRPlotWidget->updateToolbar();
	ui_.FRPlotWidget->updateToolbar();
	ui_.PhiRPlotWidget->updateToolbar();
	ui_.PhiMagPlotWidget->updateToolbar();
	ui_.RFactorPlotWidget->updateToolbar();

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
	Renderable* rFacTot = rFactorGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//RFactor", module->uniqueName()), "TotalRFactor", "Total");
	rFacTot->lineStyle().setStipple(LineStipple::HalfDashStipple);

	// Add reference data & calculated data to the FQGraph_, and percentage errors to the rFactorGraph_
	RefListIterator<Module,ModuleGroup*> targetIterator(module->allTargets());
	while (Module* targetModule = targetIterator.iterate())
	{
		// Reference data
		Renderable* refData = FQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceData", targetModule->uniqueName()), CharString("ExpFQ//%s", targetModule->uniqueName()), CharString("%s Exp", targetModule->uniqueName()));
		FQGraph_->addRenderableToGroup(refData, targetModule->uniqueName());
	
		// Calculated data from associated module
		if (DissolveSys::sameString(targetModule->type(), "NeutronSQ"))
		{
			// F(Q)
			Renderable* calcFQ = FQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//Total", targetModule->uniqueName()), CharString("CalcFQ//%s", targetModule->uniqueName()), CharString("%s Calc", targetModule->uniqueName()));
			calcFQ->lineStyle().setStipple(LineStipple::HalfDashStipple);
			FQGraph_->addRenderableToGroup(calcFQ, targetModule->uniqueName());

			// F(Q) diff w.r.t. reference
			Renderable* diffFQ = FQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//Difference//%s", module->uniqueName(), targetModule->uniqueName()), CharString("DiffFQ//%s//%s", module->uniqueName(), targetModule->uniqueName()), CharString("%s Diff", targetModule->uniqueName()));
			diffFQ->lineStyle().setStipple(LineStipple::DotStipple);
			FQGraph_->addRenderableToGroup(diffFQ, targetModule->uniqueName());

			// R-Factor between F(Q) and reference
			Renderable* rFactor = rFactorGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//RFactor//%s", module->uniqueName(), targetModule->uniqueName()), CharString("RFactor//%s//%s", module->uniqueName(), targetModule->uniqueName()), targetModule->uniqueName());
			rFactorGraph_->addRenderableToGroup(rFactor, targetModule->uniqueName());

			// Reference F(r) (from direct FT of input data)
			Renderable* expFR = FRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceDataFT", targetModule->uniqueName()), CharString("ExpFR//%s", targetModule->uniqueName()), CharString("%s Exp", targetModule->uniqueName()));
			FRGraph_->addRenderableToGroup(expFR, targetModule->uniqueName());

			// Simulated F(r) (from FT of the calculated F(Q))
			Renderable* calcFR = FRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//SimulatedFR//%s", module->uniqueName(), targetModule->uniqueName()), CharString("CalcFR//%s//%s", module->uniqueName(), targetModule->uniqueName()), CharString("%s Calc", targetModule->uniqueName()));
			calcFR->lineStyle().setStipple(LineStipple::HalfDashStipple);
			FRGraph_->addRenderableToGroup(calcFR, targetModule->uniqueName());
		}

		// Delta F(Q) and fit
		Renderable* deltaFQ = FQFitGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaFQ//%s", module->uniqueName(), targetModule->uniqueName()), CharString("DeltaFQ//%s//%s", module->uniqueName(), targetModule->uniqueName()), CharString("%s Delta", targetModule->uniqueName()));
		FQFitGraph_->addRenderableToGroup(deltaFQ, targetModule->uniqueName());

		Renderable* deltaFQFit = FQFitGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaFQFit//%s", module->uniqueName(), targetModule->uniqueName()), CharString("FitFQ//%s//%s", module->uniqueName(), targetModule->uniqueName()), CharString("%s Fit", targetModule->uniqueName()));
		deltaFQFit->lineStyle().setStipple(LineStipple::QuarterDashStipple);
		FQFitGraph_->addRenderableToGroup(deltaFQFit, targetModule->uniqueName());
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
				Renderable* expSQ = SQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//GeneratedSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("ExpSQ//%s", id.get()), CharString("%s Exp", id.get()));
				SQGraph_->addRenderableToGroup(expSQ, id.get());

				// Calculated / summed partial
				Renderable* calcSQ = SQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("CalcSQ//%s", id.get()), CharString("%s Calc", id.get()));
				calcSQ->lineStyle().setStipple(LineStipple::QuarterDashStipple);
				SQGraph_->addRenderableToGroup(calcSQ, id.get());

				// Deltas
				Renderable* deltaSQ = SQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//DeltaSQ//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("DeltaSQ//%s", id.get()), CharString("%s Delta", id.get()));
				deltaSQ->lineStyle().setStipple(LineStipple::DotStipple);
				SQGraph_->addRenderableToGroup(deltaSQ, id.get());

				/*
				 * Partial RDFs
				 */

				// Experimentally-determined unweighted partial
				Renderable* expGR = GRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//GeneratedGR//%s//%s-%s", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("ExpGR//%s", id.get()), CharString("%s Exp", id.get()));
				GRGraph_->addRenderableToGroup(expGR, id.get());

				// Calculated / summed partial
				Renderable* calcGR = GRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s//%s-%s//Full", module_->uniqueName(), group->name(), at1->name(), at2->name()), CharString("CalcGR//%s", id.get()), CharString("%s Calc", id.get()));
				calcGR->lineStyle().setStipple(LineStipple::QuarterDashStipple);
				GRGraph_->addRenderableToGroup(calcGR, id.get());
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
			Renderable* phi = phiRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("PairPotential//%s//Additional", id.get()), CharString("PhiR//%s", id.get()), CharString("%s", id.get()));
			phiRGraph_->addRenderableToGroup(phi, id.get());
		}
	}

	// Add phi magnitude data
	phiMagGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//EPMag", module_->uniqueName()), "EReq", "EReq");
}

/*
 * Debug Tab
 */

// Update data shown on fit coefficients viewer
void EPSRModuleWidget::updateDebugEPFunctionsGraph(int from, int to)
{
	DataViewer* viewer = ui_.DebugEPFunctionsPlotWidget->dataViewer();
	viewer->clearRenderables();
	debugFunctionData_.clear();

	if (!module_) return;

	int i = 0;
	for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			// Add generate potential to graph
			Renderable* phi = viewer->createRenderable(Renderable::Data1DRenderable, CharString("PairPotential//%s//Additional", id.get()), id.get(), id.get());
			viewer->addRenderableToGroup(phi, id.get());

			// Generate data for function range specified
			for (int n=from; n<=to; ++n)
			{
				Data1D* data = debugFunctionData_.add();
				(*data) = module_->generateEmpiricalPotentialFunction(dissolve_, i, j, n);
				data->setObjectTag(CharString("PairPotential//%s//Function//%i", id.get(), n));
				Renderable* rend = viewer->createRenderable(Renderable::Data1DRenderable, CharString("PairPotential//%s//Function//%i", id.get(), n), DissolveSys::itoa(n), CharString("%s/%i", id.get(), n));
				viewer->addRenderableToGroup(rend, id.get());
			}
		}
	}

}

void EPSRModuleWidget::on_DebugFromSpin_valueChanged(int value)
{
	updateDebugEPFunctionsGraph(value, ui_.DebugToSpin->value());
}

void EPSRModuleWidget::on_DebugToSpin_valueChanged(int value)
{
	updateDebugEPFunctionsGraph(ui_.DebugFromSpin->value(), value);
}

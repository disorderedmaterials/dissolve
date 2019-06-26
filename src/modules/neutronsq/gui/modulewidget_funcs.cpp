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
#include "genericitems/listhelper.h"

// Constructor
NeutronSQModuleWidget::NeutronSQModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<NeutronSQModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Set up partial g(r) graph

	partialGRGraph_ = ui.PartialGRPlotWidget->dataViewer();

	partialGRGraph_->view().setViewType(View::FlatXYView);
	partialGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	partialGRGraph_->view().axes().setMax(0, 10.0);
	partialGRGraph_->view().axes().setTitle(1, "g(r)");
	partialGRGraph_->view().axes().setMin(1, -1.0);
	partialGRGraph_->view().axes().setMax(1, 1.0);
	partialGRGraph_->groupManager().setVerticalShift(RenderableGroupManager::TwoVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up partial S(Q) graph

	partialSQGraph_ = ui.PartialSQPlotWidget->dataViewer();

	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->groupManager().setVerticalShift(RenderableGroupManager::TwoVerticalShift);
	partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph

	totalGRGraph_ = ui.TotalGRPlotWidget->dataViewer();

	totalGRGraph_->view().setViewType(View::FlatXYView);
	totalGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	totalGRGraph_->view().axes().setMax(0, 10.0);
	totalGRGraph_->view().axes().setTitle(1, "g(r)");
	totalGRGraph_->view().axes().setMin(1, -1.0);
	totalGRGraph_->view().axes().setMax(1, 1.0);
	totalGRGraph_->groupManager().setVerticalShift(RenderableGroupManager::NoVerticalShift);
	totalGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total S(Q) graph

	totalFQGraph_ = ui.TotalSQPlotWidget->dataViewer();

	totalFQGraph_->view().setViewType(View::FlatXYView);
	totalFQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	totalFQGraph_->view().axes().setMax(0, 10.0);
	totalFQGraph_->view().axes().setTitle(1, "S(Q)");
	totalFQGraph_->view().axes().setMin(1, -1.0);
	totalFQGraph_->view().axes().setMax(1, 1.0);
	totalFQGraph_->groupManager().setVerticalShift(RenderableGroupManager::NoVerticalShift);
	totalFQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	setGraphDataTargets(module_);

	updateControls();

	refreshing_ = false;
}

NeutronSQModuleWidget::~NeutronSQModuleWidget()
{
}

// Update controls within widget
void NeutronSQModuleWidget::updateControls()
{
	ui.PartialGRPlotWidget->updateToolbar();
	ui.PartialSQPlotWidget->updateToolbar();
	ui.TotalGRPlotWidget->updateToolbar();
	ui.TotalSQPlotWidget->updateToolbar();

	partialGRGraph_->postRedisplay();
	partialSQGraph_->postRedisplay();
	totalGRGraph_->postRedisplay();
	totalFQGraph_->postRedisplay();
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
	if (!totalFQGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool NeutronSQModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!partialGRGraph_->readSession(parser)) return false;
	if (!partialSQGraph_->readSession(parser)) return false;
	if (!totalGRGraph_->readSession(parser)) return false;
	if (!totalFQGraph_->readSession(parser)) return false;
	
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
			Renderable* fullGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s-%s//Full", module_->uniqueName(), at1->name(), at2->name()), CharString("Total//%s", id.get()), id.get());
			partialGRGraph_->addRenderableToGroup(fullGR, id.get());

			// Bound partial
			Renderable* boundGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s-%s//Bound", module_->uniqueName(), at1->name(), at2->name()), CharString("Bound//%s", id.get()), CharString("%s Bound", id.get()));
			boundGR->lineStyle().setStipple(LineStipple::HalfDashStipple);
			partialGRGraph_->addRenderableToGroup(boundGR, id.get());

			// Unbound partial
			Renderable* unboundGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s-%s//Unbound", module_->uniqueName(), at1->name(), at2->name()), CharString("Unbound//%s", id.get()), CharString("%s Unbound", id.get()));
			unboundGR->lineStyle().setStipple(LineStipple::DotStipple);
			partialGRGraph_->addRenderableToGroup(unboundGR, id.get());

			/*
			 * Partial S(Q)
			 */

			// Full partial
			Renderable* fullSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s-%s//Full", module_->uniqueName(), at1->name(), at2->name()), CharString("Total//%s", id.get()), id.get());
			partialSQGraph_->addRenderableToGroup(fullSQ, id.get());

			// Bound partial
			Renderable* boundSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s-%s//Bound", module_->uniqueName(), at1->name(), at2->name()), CharString("Bound//%s", id.get()), CharString("%s Bound", id.get()));
			boundSQ->lineStyle().setStipple(LineStipple::HalfDashStipple);
			partialSQGraph_->addRenderableToGroup(boundSQ, id.get());

			// Unbound partial
			Renderable* unboundSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s-%s//Unbound", module_->uniqueName(), at1->name(), at2->name()), CharString("Unbound//%s", id.get()), CharString("%s Unbound", id.get()));
			unboundSQ->lineStyle().setStipple(LineStipple::DotStipple);
			partialSQGraph_->addRenderableToGroup(unboundSQ, id.get());
		}
	}

	// Add calculated total G(r)
	Renderable* totalGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//Total", module_->uniqueName()), "Calculated");
	totalGRGraph_->addRenderableToGroup(totalGR, "Calc");

	// Add calculate total F(Q)
	Renderable* totalFQ = totalFQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//Total", module_->uniqueName()), "Calculated");
	totalFQGraph_->addRenderableToGroup(totalFQ, "Calc");

	// Add on reference data if present
	if (module->keywords().find("Reference"))
	{
		// Add FT of reference data total G(r)
		Renderable* refGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceDataFT", module_->uniqueName()), "Reference");
		totalGRGraph_->addRenderableToGroup(refGR, "Reference");

		// Add calculate total F(Q)
		Renderable* refFQ = totalFQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceData", module_->uniqueName()), "Reference");
		totalFQGraph_->addRenderableToGroup(refFQ, "Reference");
	}
}

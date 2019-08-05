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
	partialGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up partial S(Q) graph

	partialSQGraph_ = ui.PartialSQPlotWidget->dataViewer();

	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph

	totalGRGraph_ = ui.TotalGRPlotWidget->dataViewer();

	totalGRGraph_->view().setViewType(View::FlatXYView);
	totalGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	totalGRGraph_->view().axes().setMax(0, 10.0);
	totalGRGraph_->view().axes().setTitle(1, "g(r)");
	totalGRGraph_->view().axes().setMin(1, -1.0);
	totalGRGraph_->view().axes().setMax(1, 1.0);
	totalGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
	totalGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total S(Q) graph

	totalFQGraph_ = ui.TotalSQPlotWidget->dataViewer();

	totalFQGraph_->view().setViewType(View::FlatXYView);
	totalFQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	totalFQGraph_->view().axes().setMax(0, 10.0);
	totalFQGraph_->view().axes().setTitle(1, "S(Q)");
	totalFQGraph_->view().axes().setMin(1, -1.0);
	totalFQGraph_->view().axes().setMax(1, 1.0);
	totalFQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
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
			partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s//Full", module_->uniqueName(), id.get()), CharString("%s (Full)", id.get()), "Full");

			// Bound partial
			partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s//Bound", module_->uniqueName(), id.get()), CharString("%s (Bound)", id.get()), "Bound");

			// Unbound partial
			partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//%s//Unbound", module_->uniqueName(), id.get()), CharString("%s (Unbound)", id.get()), "Unbound");

			/*
			 * Partial S(Q)
			 */

			// Full partial
			partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s//Full", module_->uniqueName(), id.get()), CharString("%s (Full)", id.get()), "Full");

			// Bound partial
			partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s//Bound", module_->uniqueName(), id.get()), CharString("%s (Bound)", id.get()), "Bound");

			// Unbound partial
			partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//%s//Unbound", module_->uniqueName(), id.get()), CharString("%s (Unbound)", id.get()), "Unbound");
		}
	}

	// Set group styling
	partialGRGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
	partialGRGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
	partialGRGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
	partialGRGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
	partialGRGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
	partialGRGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
	partialGRGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
	partialGRGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);
	partialSQGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
	partialSQGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
	partialSQGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
	partialSQGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
	partialSQGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
	partialSQGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
	partialSQGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
	partialSQGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

	// Add calculated total G(r)
	Renderable* totalGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//Total", module_->uniqueName()), "Calculated");
	totalGRGraph_->addRenderableToGroup(totalGR, "Calculated");

	// Add calculate total F(Q)
	Renderable* totalFQ = totalFQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//Total", module_->uniqueName()), "Calculated");
	totalFQGraph_->addRenderableToGroup(totalFQ, "Calculated");

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

	// Set group styling
	totalGRGraph_->groupManager().setGroupColouring("Reference", RenderableGroup::FixedGroupColouring);
	totalGRGraph_->groupManager().setGroupFixedColour("Reference", ColourDefinition::RedStockColour);
	totalFQGraph_->groupManager().setGroupColouring("Reference", RenderableGroup::FixedGroupColouring);
	totalFQGraph_->groupManager().setGroupFixedColour("Reference", ColourDefinition::RedStockColour);
}

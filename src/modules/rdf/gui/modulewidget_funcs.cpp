/*
	*** RDF Module Widget - Functions
	*** src/modules/rdf/gui/modulewidget_funcs.cpp
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

#include "modules/rdf/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
RDFModuleWidget::RDFModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<RDFModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Set up partial g(r) graph

	partialsGraph_ = ui.PartialsPlotWidget->dataViewer();

	partialsGraph_->view().setViewType(View::FlatXYView);
	partialsGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	partialsGraph_->view().axes().setMax(0, 10.0);
	partialsGraph_->view().axes().setTitle(1, "g(r)");
	partialsGraph_->view().axes().setMin(1, -1.0);
	partialsGraph_->view().axes().setMax(1, 1.0);
	partialsGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
	partialsGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph

	totalsGraph_ = ui.TotalsPlotWidget->dataViewer();

	totalsGraph_->view().setViewType(View::FlatXYView);
	totalsGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	totalsGraph_->view().axes().setMax(0, 10.0);
	totalsGraph_->view().axes().setTitle(1, "g(r)");
	totalsGraph_->view().axes().setMin(1, -1.0);
	totalsGraph_->view().axes().setMax(1, 1.0);
	totalsGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);
	totalsGraph_->view().setAutoFollowType(View::AllAutoFollow);

	refreshing_ = false;

	currentConfiguration_ = NULL;

	updateControls();

	setGraphDataTargets(module_);
}

RDFModuleWidget::~RDFModuleWidget()
{
}

// Update controls within widget
void RDFModuleWidget::updateControls()
{
	ui.PartialsPlotWidget->updateToolbar();
	ui.TotalsPlotWidget->updateToolbar();

	partialsGraph_->postRedisplay();
	totalsGraph_->postRedisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void RDFModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void RDFModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool RDFModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!partialsGraph_->writeSession(parser)) return false;
	if (!totalsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool RDFModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!partialsGraph_->readSession(parser)) return false;
	if (!totalsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void RDFModuleWidget::setGraphDataTargets(RDFModule* module)
{
	if (!module) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));

	// Loop over Configurations and add total G(R)
	CharString blockData;
	configIterator.restart();
	while (Configuration* cfg = configIterator.iterate())
	{
		// Add calculated total G(r)
		Renderable* refData = totalsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//Total", cfg->niceName()), CharString("Calculated//%s", cfg->niceName()), cfg->niceName());
		totalsGraph_->addRenderableToGroup(refData, "Calc");
	}
}

void RDFModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current data
	partialsGraph_->clearRenderables();

	// Get target Configuration
	currentConfiguration_ = VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
	if (!currentConfiguration_) return;

	CharString blockData;
	const AtomTypeList cfgTypes = currentConfiguration_->usedAtomTypesList();
	int n = 0;
	for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			// Full partial
			Renderable* fullGR = partialsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s//Full", currentConfiguration_->niceName(), id.get()), CharString("Full//%s", id.get()), id.get(), "Full");

			// Bound partial
			Renderable* boundGR = partialsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s//Bound", currentConfiguration_->niceName(), id.get()), CharString("Bound//%s", id.get()), id.get(), "Bound");
			boundGR->lineStyle().setStipple(LineStipple::HalfDashStipple);

			// Unbound partial
			Renderable* unboundGR = partialsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s//Unbound", currentConfiguration_->niceName(), id.get()), CharString("Unbound//%s", id.get()), id.get(), "Unbound");
			unboundGR->lineStyle().setStipple(LineStipple::DotStipple);
		}
	}

	// Set group styling
	partialsGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
	partialsGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
	partialsGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
	partialsGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
	partialsGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
	partialsGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
}

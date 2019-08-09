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

#include "modules/bragg/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
BraggModuleWidget::BraggModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<BraggModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Set up Bragg reflections graph
	reflectionsGraph_ = ui.ReflectionsPlotWidget->dataViewer();
	// -- Set view
	reflectionsGraph_->view().setViewType(View::FlatXYView);
	reflectionsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	reflectionsGraph_->view().axes().setMax(0, 10.0);
	reflectionsGraph_->view().axes().setTitle(1, "Intensity");
	reflectionsGraph_->view().axes().setMin(1, -1.0);
	reflectionsGraph_->view().axes().setMax(1, 1.0);
	reflectionsGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph
	totalsGraph_ = ui.TotalsPlotWidget->dataViewer();
	// -- Set view
	totalsGraph_->view().setViewType(View::FlatXYView);
	totalsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	totalsGraph_->view().axes().setMax(0, 10.0);
	totalsGraph_->view().axes().setTitle(1, "Intensity");
	totalsGraph_->view().axes().setMin(1, -1.0);
	totalsGraph_->view().axes().setMax(1, 1.0);
	totalsGraph_->view().setAutoFollowType(View::AllAutoFollow);
	// -- Set group styling
	totalsGraph_->groupManager().setGroupColouring("Totals", RenderableGroup::AutomaticIndividualColouring);

	refreshing_ = false;

	currentConfiguration_ = NULL;

	updateControls();

	setGraphDataTargets();
}

BraggModuleWidget::~BraggModuleWidget()
{
}

// Update controls within widget
void BraggModuleWidget::updateControls()
{
	ui.ReflectionsPlotWidget->updateToolbar();
	ui.TotalsPlotWidget->updateToolbar();

	reflectionsGraph_->postRedisplay();
	totalsGraph_->postRedisplay();
}

// Disable sensitive controls within widget
void BraggModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void BraggModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool BraggModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!reflectionsGraph_->writeSession(parser)) return false;
	if (!totalsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool BraggModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!reflectionsGraph_->readSession(parser)) return false;
	if (!totalsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void BraggModuleWidget::setGraphDataTargets()
{
	if (!module_) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration> configIterator(module_->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));

	// Loop over Configurations and add total Bragg F(Q)
	CharString blockData;
	configIterator.restart();
	while (Configuration* cfg = configIterator.iterate())
	{
		// Original F(Q)
		Renderable* originalFQ = totalsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//OriginalBragg//Total", cfg->niceName()), cfg->niceName(), "Totals");
	}
}

void BraggModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current data
	reflectionsGraph_->clearRenderables();

	// Get target Configuration
	currentConfiguration_ = (Configuration*) VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
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

			// Original S(Q)
			Renderable* originalSQ = reflectionsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//OriginalBragg//%s", currentConfiguration_->niceName(), id.get()), CharString("Full//%s", id.get()), "Full");
		}
	}
	reflectionsGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
}

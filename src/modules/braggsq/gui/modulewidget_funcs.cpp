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

#include "modules/braggsq/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/braggsq/braggsq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
BraggSQModuleWidget::BraggSQModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((BraggSQModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Set up Bragg S(Q) graph

	sqGraph_ = ui.BraggSQPlotWidget->dataViewer();

	sqGraph_->view().setViewType(View::FlatXYView);
	sqGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	sqGraph_->view().axes().setMax(0, 10.0);
	sqGraph_->view().axes().setTitle(1, "Intensity");
	sqGraph_->view().axes().setMin(1, -1.0);
	sqGraph_->view().axes().setMax(1, 1.0);
	sqGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph

	fqGraph_ = ui.BraggFQPlotWidget->dataViewer();

	fqGraph_->view().setViewType(View::FlatXYView);
	fqGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	fqGraph_->view().axes().setMax(0, 10.0);
	fqGraph_->view().axes().setTitle(1, "Intensity");
	fqGraph_->view().axes().setMin(1, -1.0);
	fqGraph_->view().axes().setMax(1, 1.0);
	fqGraph_->view().setAutoFollowType(View::AllAutoFollow);

	refreshing_ = false;

	currentConfiguration_ = NULL;

	updateControls();

	setGraphDataTargets();
}

BraggSQModuleWidget::~BraggSQModuleWidget()
{
}

// Update controls within widget
void BraggSQModuleWidget::updateControls()
{
	ui.BraggSQPlotWidget->updateToolbar();
	ui.BraggFQPlotWidget->updateToolbar();

	sqGraph_->postRedisplay();
	fqGraph_->postRedisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void BraggSQModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void BraggSQModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool BraggSQModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!sqGraph_->writeSession(parser)) return false;
	if (!fqGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool BraggSQModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!sqGraph_->readSession(parser)) return false;
	if (!fqGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void BraggSQModuleWidget::setGraphDataTargets()
{
	if (!module_) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));

	// Loop over Configurations and add total Bragg F(Q)
	CharString blockData;
	configIterator.restart();
	while (Configuration* cfg = configIterator.iterate())
	{
		// Original F(Q)
		Renderable* originalFQ = fqGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//OriginalBraggFQ//Total", cfg->niceName()), cfg->niceName(), cfg->niceName());
		fqGraph_->groupManager().addToGroup(originalFQ, cfg->niceName());
	}
}

void BraggSQModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current data
	sqGraph_->clearRenderables();

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
			Renderable* originalSQ = sqGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//OriginalBraggSQ//%s-%s", currentConfiguration_->niceName(), at1->name(), at2->name()), CharString("Full//%s", id.get()), id.get());
			sqGraph_->groupManager().addToGroup(originalSQ, id.get());
		}
	}
}

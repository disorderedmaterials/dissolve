/*
	*** RDF Module Widget - Functions
	*** src/modules/rdf/gui/modulewidget_funcs.cpp
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

#include "modules/bragg/gui/modulewidget.h"
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
BraggModuleWidget::BraggModuleWidget(QWidget* parent, BraggModule* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui_.setupUi(this);

	// Set up Bragg reflections graph
	reflectionsGraph_ = ui_.ReflectionsPlotWidget->dataViewer();
	// -- Set view
	reflectionsGraph_->view().setViewType(View::FlatXYView);
	reflectionsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	reflectionsGraph_->view().axes().setMax(0, 10.0);
	reflectionsGraph_->view().axes().setTitle(1, "Intensity");
	reflectionsGraph_->view().axes().setMin(1, -1.0);
	reflectionsGraph_->view().axes().setMax(1, 1.0);
	reflectionsGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up total G(r) graph
	totalsGraph_ = ui_.TotalsPlotWidget->dataViewer();
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
void BraggModuleWidget::updateControls(int flags)
{
	ui_.ReflectionsPlotWidget->updateToolbar();
	ui_.TotalsPlotWidget->updateToolbar();

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
 * State I/O
 */

// Write widget state through specified LineParser
bool BraggModuleWidget::writeState(LineParser& parser) const
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
	ui_.TargetCombo->clear();
	for (Configuration* config : module_->targetConfigurations()) ui_.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));

	// Loop over Configurations and add total Bragg F(Q)
	CharString blockData;
	for (Configuration* cfg : module_->targetConfigurations())
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
	currentConfiguration_ = (Configuration*) VariantPointer<Configuration>(ui_.TargetCombo->itemData(index));
	if (!currentConfiguration_) return;

	CharString blockData;
	const AtomTypeList cfgTypes = currentConfiguration_->usedAtomTypesList();
	int n = 0;
	for (AtomTypeData* atd1 = cfgTypes.first(); atd1 != NULL; atd1 = atd1->next(), ++n)
	{
		int m = n;
		for (AtomTypeData* atd2 = atd1; atd2 != NULL; atd2 = atd2->next(), ++m)
		{
			CharString id("%s-%s", atd1->atomTypeName(), atd2->atomTypeName());

			// Original S(Q)
			Renderable* originalSQ = reflectionsGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//OriginalBragg//%s", currentConfiguration_->niceName(), id.get()), CharString("Full//%s", id.get()), "Full");
		}
	}
	reflectionsGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
}

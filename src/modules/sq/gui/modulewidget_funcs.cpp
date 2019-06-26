/*
	*** SQ Module Widget - Functions
	*** src/modules/sq/gui/modulewidget_funcs.cpp
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

#include "modules/sq/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/sq/sq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
SQModuleWidget::SQModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<SQModule*>(module)), dissolve_(dissolve)
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
	partialGRGraph_->groupManager().setVerticalShift(RenderableGroupManager::HalfVerticalShift);
	partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);

	// Set up partial S(Q) graph

	partialSQGraph_ = ui.PartialSQPlotWidget->dataViewer();

	partialSQGraph_->view().setViewType(View::FlatXYView);
	partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	partialSQGraph_->view().axes().setMax(0, 10.0);
	partialSQGraph_->view().axes().setTitle(1, "S(Q)");
	partialSQGraph_->view().axes().setMin(1, -1.0);
	partialSQGraph_->view().axes().setMax(1, 1.0);
	partialSQGraph_->groupManager().setVerticalShift(RenderableGroupManager::HalfVerticalShift);
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

	totalSQGraph_ = ui.TotalSQPlotWidget->dataViewer();

	totalSQGraph_->view().setViewType(View::FlatXYView);
	totalSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
	totalSQGraph_->view().axes().setMax(0, 10.0);
	totalSQGraph_->view().axes().setTitle(1, "S(Q)");
	totalSQGraph_->view().axes().setMin(1, -1.0);
	totalSQGraph_->view().axes().setMax(1, 1.0);
	totalSQGraph_->groupManager().setVerticalShift(RenderableGroupManager::NoVerticalShift);
	totalSQGraph_->view().setAutoFollowType(View::AllAutoFollow);

	setGraphDataTargets(module_);

	updateControls();

	refreshing_ = false;
}

SQModuleWidget::~SQModuleWidget()
{
}

// Update controls within widget
void SQModuleWidget::updateControls()
{
	ui.PartialGRPlotWidget->updateToolbar();
	ui.PartialSQPlotWidget->updateToolbar();
	ui.TotalGRPlotWidget->updateToolbar();
	ui.TotalSQPlotWidget->updateToolbar();

	partialGRGraph_->postRedisplay();
	partialSQGraph_->postRedisplay();
	totalGRGraph_->postRedisplay();
	totalSQGraph_->postRedisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void SQModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void SQModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool SQModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!partialGRGraph_->writeSession(parser)) return false;
	if (!partialSQGraph_->writeSession(parser)) return false;
	if (!totalGRGraph_->writeSession(parser)) return false;
	if (!totalSQGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool SQModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!partialGRGraph_->readSession(parser)) return false;
	if (!partialSQGraph_->readSession(parser)) return false;
	if (!totalGRGraph_->readSession(parser)) return false;
	if (!totalSQGraph_->readSession(parser)) return false;
	
	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void SQModuleWidget::setGraphDataTargets(SQModule* module)
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

			// Partial g(r)

			Renderable* fullGR = partialGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedGR//%s-%s//Full", module_->uniqueName(), at1->name(), at2->name()), CharString("GR//%s", id.get()), id.get());
			partialGRGraph_->addRenderableToGroup(fullGR, id.get());

			// Partial S(Q)

			Renderable* fullSQ = partialSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//UnweightedSQ//%s-%s//Full", module_->uniqueName(), at1->name(), at2->name()), CharString("SQ//%s", id.get()), id.get());
			partialSQGraph_->addRenderableToGroup(fullSQ, id.get());
		}
	}

	// Add calculated total G(r)
	Renderable* totalGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedGR//Total", module_->uniqueName()), "G(r) Calc");
	totalGRGraph_->addRenderableToGroup(totalGR, "Calc");

	// Add calculate total F(Q)
	Renderable* totalFQ = totalSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//WeightedSQ//Total", module_->uniqueName()), "F(Q) Calc");
	totalSQGraph_->addRenderableToGroup(totalFQ, "Calc");

	// Add on reference data if present
	if (module->keywords().find("Reference"))
	{
		// Add FT of reference data total G(r)
		Renderable* refGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceDataFT", module_->uniqueName()), "G(r) Exp");
		totalGRGraph_->addRenderableToGroup(refGR, "Exp");

		// Add calculate total F(Q)
		Renderable* refFQ = totalSQGraph_->createRenderable(Renderable::Data1DRenderable, CharString("%s//ReferenceData", module_->uniqueName()), "F(Q) Exp");
		totalSQGraph_->addRenderableToGroup(refFQ, "Exp");
	}
}

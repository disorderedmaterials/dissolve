/*
	*** CalculateCN Module Widget - Functions
	*** src/modules/calculate_cn/gui/modulewidget_funcs.cpp
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

#include "modules/calculate_cn/gui/modulewidget.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"

// Constructor
CalculateCNModuleWidget::CalculateCNModuleWidget(QWidget* parent, CalculateCNModule* cnModule) : ModuleWidget(parent), module_(cnModule)
{
	// Set up user interface
	ui_.setupUi(this);

	// Set up RDF graph
	rdfGraph_ = ui_.RDFPlotWidget;

	View& view = rdfGraph_->view();
	view.setViewType(View::FlatXYView);
	view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	view.axes().setMax(0, 10.0);
	view.axes().setTitle(1, "g(r)");
	view.axes().setMin(1, 0.0);
	view.axes().setMax(1, 1.0);
	view.setAutoFollowType(View::AllAutoFollow);

	rdfDataLocated_ = false;

	setGraphDataTargets();

	refreshing_ = false;
}

// Update controls within widget
void CalculateCNModuleWidget::updateControls(int flags)
{
	// Update CN labels
	if (module_)
	{
		ui_.RegionAResultFrame->setText(module_->coordinationNumber(0));
		ui_.RegionBResultFrame->setText(module_->coordinationNumber(1));
		ui_.RegionCResultFrame->setText(module_->coordinationNumber(2));
	}
	else
	{
		ui_.RegionAResultFrame->setText(SampledDouble());
		ui_.RegionBResultFrame->setText(SampledDouble());
		ui_.RegionCResultFrame->setText(SampledDouble());
	}

	// Clear and recreate graph data targets?
	if ((!rdfDataLocated_) || (flags&ModuleWidget::ResetGraphDataTargetsFlag)) setGraphDataTargets();

	rdfGraph_->postRedisplay();
}

// Disable sensitive controls within widget
void CalculateCNModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void CalculateCNModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool CalculateCNModuleWidget::writeState(LineParser& parser) const
{
	return false;
}

// Read widget state through specified LineParser
bool CalculateCNModuleWidget::readState(LineParser& parser)
{
	return false;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateCNModuleWidget::setGraphDataTargets()
{
	rdfGraph_->clearRenderables();

	// Get target RDF module
	bool found = false;
	const CalculateRDFModule* rdfModule = module_->keywords().retrieve<const CalculateRDFModule*>("SourceRDF", NULL, &found);

	// If the RDF data for the graph has not yet been found, attempt to locate it now
	if (!rdfDataLocated_)
	{
		if ((!found) || (!rdfModule))
		{
			Messenger::warn("No suitable CalculateRDF target set for CalculateCN, so can't plot RDF.\n");
			return;
		}

		// Check that processed ata exists in the RDF module
		if (!rdfModule->rdfResult()) return;
	}

	// Set RDF data target
	Renderable* rdfRenderable = rdfGraph_->createRenderable(Renderable::Data1DRenderable, rdfModule->rdfResult()->processedData().objectTag(), rdfModule->rdfResult()->processedData().name());

	rdfDataLocated_ = true;
}

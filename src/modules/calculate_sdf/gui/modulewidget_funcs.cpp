/*
	*** CalculateSDF Module Widget - Functions
	*** src/modules/calculate_sdf/gui/modulewidget_funcs.cpp
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

#include "modules/calculate_sdf/gui/modulewidget.h"
#include "modules/calculate_sdf/sdf.h"
#include "classes/configuration.h"

// Constructor
CalculateSDFModuleWidget::CalculateSDFModuleWidget(QWidget* parent, CalculateSDFModule* module) : ModuleWidget(parent), module_(module)
{
	// Set up user interface
	ui_.setupUi(this);

	// Set up RDF graph
	sdfGraph_ = ui_.SDFPlotWidget->dataViewer();

	View& sdfView = sdfGraph_->view();
	sdfView.setViewType(View::NormalView);
	sdfView.axes().setTitle(0, "X, \\sym{angstrom}");
	sdfView.axes().setRange(0, -10.0, 10.0);
	sdfView.axes().setTitle(1, "Y, \\sym{angstrom}");
	sdfView.axes().setRange(1, -10.0, 10.0);
	sdfView.axes().setTitle(2, "Z, \\sym{angstrom}");
	sdfView.axes().setRange(2, -10.0, 10.0);
	sdfView.setAutoFollowType(View::AllAutoFollow);

	setGraphDataTargets();

	refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void CalculateSDFModuleWidget::updateControls(int flags)
{
	ui_.SDFPlotWidget->updateToolbar();

	sdfGraph_->postRedisplay();
}

// Disable sensitive controls within widget
void CalculateSDFModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void CalculateSDFModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool CalculateSDFModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!sdfGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool CalculateSDFModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!sdfGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateSDFModuleWidget::setGraphDataTargets()
{
	// Remove any current data
	sdfGraph_->clearRenderables();

	if (!module_) return;

	// Loop over Configuration targets in Module
	RefListIterator<Configuration> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		// Calculated SDF
		Renderable* sdf = sdfGraph_->createRenderable(Renderable::Data3DRenderable, CharString("%s//Process3D//%s//SDF", module_->uniqueName(), cfg->niceName()), CharString("SDF//%s", cfg->niceName()), cfg->niceName());
		sdf->setColour(StockColours::BlueStockColour);
	}
}

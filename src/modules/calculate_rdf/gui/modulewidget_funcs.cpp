/*
    *** CalculateRDF Module Widget - Functions
    *** src/modules/calculate_rdf/gui/modulewidget_funcs.cpp
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

#include "classes/configuration.h"
#include "modules/calculate_rdf/gui/modulewidget.h"
#include "modules/calculate_rdf/rdf.h"

CalculateRDFModuleWidget::CalculateRDFModuleWidget(QWidget *parent, CalculateRDFModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget->dataViewer();

    auto &view = rdfGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "g(r)");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets();

    updateControls();

    refreshing_ = false;
}

// Update controls within widget
void CalculateRDFModuleWidget::updateControls(int flags)
{
    ui_.RDFPlotWidget->updateToolbar();

    rdfGraph_->postRedisplay();
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool CalculateRDFModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!rdfGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool CalculateRDFModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!rdfGraph_->readSession(parser))
        return false;

    return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateRDFModuleWidget::setGraphDataTargets()
{
    // Remove any current data
    rdfGraph_->clearRenderables();

    if (!module_)
        return;

    // Loop over Configuration targets in Module
    for (const auto *cfg : module_->targetConfigurations())
    {
        // Calculated RDF
        auto *rdf = rdfGraph_->createRenderable(Renderable::Data1DRenderable,
                                                fmt::format("{}//Process1D//{}//RDF", module_->uniqueName(), cfg->niceName()),
                                                fmt::format("RDF//{}", cfg->niceName()), cfg->niceName());
        rdf->setColour(StockColours::BlueStockColour);
    }
}

/*
    *** CalculateDAngle Module Widget - Functions
    *** src/modules/calculate_axisangle/gui/modulewidget_funcs.cpp
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
#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_axisangle/gui/modulewidget.h"

CalculateAxisAngleModuleWidget::CalculateAxisAngleModuleWidget(QWidget *parent, CalculateAxisAngleModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget->dataViewer();

    View &rdfView = rdfGraph_->view();
    rdfView.setViewType(View::FlatXYView);
    rdfView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    rdfView.axes().setMax(0, 10.0);
    rdfView.axes().setTitle(1, "g(r)");
    rdfView.axes().setMin(1, 0.0);
    rdfView.axes().setMax(1, 1.0);
    rdfView.setAutoFollowType(View::AllAutoFollow);

    // Set up Angle graph
    angleGraph_ = ui_.AnglePlotWidget->dataViewer();

    View &angleView = angleGraph_->view();
    angleView.setViewType(View::FlatXYView);
    angleView.axes().setTitle(0, "Angle, \\sym{degree}");
    angleView.axes().setRange(0, 0.0, 180.0);
    angleView.axes().setTitle(1, "Normalised Population");
    angleView.axes().setRange(1, 0.0, 5.0);
    angleView.setAutoFollowType(View::AllAutoFollow);

    // Set up Axis Angle graph
    dAngleGraph_ = ui_.DAnglePlotWidget->dataViewer();

    View &dAngleView = dAngleGraph_->view();
    dAngleView.setViewType(View::FlatXYView);
    dAngleView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    dAngleView.axes().setRange(0, 0.0, 5.0);
    dAngleView.axes().setTitle(1, "Angle, \\sym{degree}");
    dAngleView.axes().setRange(1, 0.0, 180.0);
    dAngleView.axes().setTitle(2, "Normalised Population");
    dAngleView.axes().setRange(2, 0.0, 0.01);
    dAngleView.setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void CalculateAxisAngleModuleWidget::updateControls(int flags)
{
    ui_.RDFPlotWidget->updateToolbar();
    ui_.AnglePlotWidget->updateToolbar();
    ui_.DAnglePlotWidget->updateToolbar();

    rdfGraph_->postRedisplay();
    angleGraph_->postRedisplay();
    dAngleGraph_->postRedisplay();
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool CalculateAxisAngleModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!rdfGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool CalculateAxisAngleModuleWidget::readState(LineParser &parser)
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
void CalculateAxisAngleModuleWidget::setGraphDataTargets(CalculateAxisAngleModule *module)
{
    // Remove any current data
    rdfGraph_->clearRenderables();

    // Loop over Configuration targets in Module
    for (const auto *cfg : module_->targetConfigurations())
    {
        // Calculated A...B RDF
        auto *rdf = rdfGraph_->createRenderable(
            Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//RDF(AB)", module_->uniqueName(), cfg->niceName()),
            "A...B g(r)");
        rdf->setColour(StockColours::BlueStockColour);

        // Calculated angle histogram
        auto *angle = angleGraph_->createRenderable(
            Renderable::Data1DRenderable,
            fmt::format("{}//Process1D//{}//AxisAngle(AB)", module_->uniqueName(), cfg->niceName()), "Axis Angle");
        angle->setColour(StockColours::RedStockColour);

        // Calculated distance-angle map
        dAngleGraph_->createRenderable(Renderable::Data2DRenderable,
                                       fmt::format("{}//Process2D//{}//DAxisAngle", module_->uniqueName(), cfg->niceName()),
                                       "A...B vs Axis Angle");
    }
}

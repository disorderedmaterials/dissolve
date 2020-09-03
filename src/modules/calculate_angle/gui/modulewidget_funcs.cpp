/*
    *** CalculateAngle Module Widget - Functions
    *** src/modules/calculate_angle/gui/modulewidget_funcs.cpp
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
#include "modules/calculate_angle/angle.h"
#include "modules/calculate_angle/gui/modulewidget.h"

CalculateAngleModuleWidget::CalculateAngleModuleWidget(QWidget *parent, CalculateAngleModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF A-B graph
    rdfABGraph_ = ui_.RDFABPlotWidget->dataViewer();

    View &rdfABView = rdfABGraph_->view();
    rdfABView.setViewType(View::FlatXYView);
    rdfABView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    rdfABView.axes().setMax(0, 10.0);
    rdfABView.axes().setTitle(1, "g(r)");
    rdfABView.axes().setMin(1, 0.0);
    rdfABView.axes().setMax(1, 1.0);
    rdfABView.setAutoFollowType(View::AllAutoFollow);

    // Set up RDF B-C graph
    rdfBCGraph_ = ui_.RDFBCPlotWidget->dataViewer();

    View &rdfBCView = rdfBCGraph_->view();
    rdfBCView.setViewType(View::FlatXYView);
    rdfBCView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    rdfBCView.axes().setMax(0, 10.0);
    rdfBCView.axes().setTitle(1, "g(r)");
    rdfBCView.axes().setMin(1, 0.0);
    rdfBCView.axes().setMax(1, 1.0);
    rdfBCView.setAutoFollowType(View::AllAutoFollow);

    // Set up Angle graph
    angleGraph_ = ui_.AnglePlotWidget->dataViewer();

    View &angleView = angleGraph_->view();
    angleView.setViewType(View::FlatXYView);
    angleView.axes().setTitle(0, "Angle, \\sym{degree}");
    angleView.axes().setRange(0, 0.0, 180.0);
    angleView.axes().setTitle(1, "Normalised Population");
    angleView.axes().setRange(1, 0.0, 5.0);
    angleView.setAutoFollowType(View::AllAutoFollow);

    // Set up Distance(AB)-Angle graph
    dAngleABGraph_ = ui_.DAngleABPlotWidget->dataViewer();

    View &dAngleABView = dAngleABGraph_->view();
    dAngleABView.setViewType(View::FlatXYView);
    dAngleABView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    dAngleABView.axes().setRange(0, 0.0, 5.0);
    dAngleABView.axes().setTitle(1, "Angle, \\sym{degree}");
    dAngleABView.axes().setRange(1, 0.0, 180.0);
    dAngleABView.axes().setTitle(2, "Normalised Population");
    dAngleABView.axes().setRange(2, 0.0, 0.01);
    dAngleABView.setAutoFollowType(View::AllAutoFollow);

    // Set up Distance(BC)-Angle graph
    dAngleBCGraph_ = ui_.DAngleBCPlotWidget->dataViewer();

    View &dAngleBCView = dAngleBCGraph_->view();
    dAngleBCView.setViewType(View::FlatXYView);
    dAngleBCView.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    dAngleBCView.axes().setRange(0, 0.0, 5.0);
    dAngleBCView.axes().setTitle(1, "Angle, \\sym{degree}");
    dAngleBCView.axes().setRange(1, 0.0, 180.0);
    dAngleBCView.axes().setTitle(2, "Normalised Population");
    dAngleBCView.axes().setRange(2, 0.0, 0.01);
    dAngleBCView.setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void CalculateAngleModuleWidget::updateControls(int flags)
{
    ui_.RDFABPlotWidget->updateToolbar();
    ui_.RDFBCPlotWidget->updateToolbar();
    ui_.AnglePlotWidget->updateToolbar();
    ui_.DAngleABPlotWidget->updateToolbar();
    ui_.DAngleBCPlotWidget->updateToolbar();

    rdfABGraph_->postRedisplay();
    rdfBCGraph_->postRedisplay();
    angleGraph_->postRedisplay();
    dAngleABGraph_->postRedisplay();
    dAngleBCGraph_->postRedisplay();
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool CalculateAngleModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!rdfABGraph_->writeSession(parser))
        return false;
    if (!rdfBCGraph_->writeSession(parser))
        return false;
    if (!angleGraph_->writeSession(parser))
        return false;
    if (!dAngleABGraph_->writeSession(parser))
        return false;
    if (!dAngleBCGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool CalculateAngleModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!rdfABGraph_->readSession(parser))
        return false;
    if (!rdfBCGraph_->readSession(parser))
        return false;
    if (!angleGraph_->readSession(parser))
        return false;
    if (!dAngleABGraph_->readSession(parser))
        return false;
    if (!dAngleBCGraph_->readSession(parser))
        return false;

    return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateAngleModuleWidget::setGraphDataTargets(CalculateAngleModule *module)
{
    // Remove any current data
    rdfABGraph_->clearRenderables();
    rdfBCGraph_->clearRenderables();
    angleGraph_->clearRenderables();
    dAngleABGraph_->clearRenderables();
    dAngleBCGraph_->clearRenderables();

    // Get Configuration target
    Configuration *cfg = module_->targetConfigurations().firstItem();
    if (!cfg)
        return;

    // Calculated A...B RDF
    auto *rdfAB = rdfABGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//RDF(AB)", module_->uniqueName(), cfg->niceName()),
        "B...C g(r)");
    rdfAB->setColour(StockColours::BlueStockColour);

    // Calculated B...C RDF
    auto *rdfBC = rdfBCGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//RDF(BC)", module_->uniqueName(), cfg->niceName()),
        "B...C g(r)");
    rdfBC->setColour(StockColours::BlueStockColour);

    // Calculated angle histogram
    auto *angle = angleGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//Angle(ABC)", module_->uniqueName(), cfg->niceName()),
        "A-B...C Angle");
    angle->setColour(StockColours::RedStockColour);

    // Calculated (A-B)-C distance-angle map
    auto *dAngleAB = dAngleABGraph_->createRenderable(
        Renderable::Data2DRenderable, fmt::format("{}//Process2D//{}//DAngle((A-B)-C)", module_->uniqueName(), cfg->niceName()),
        "A-B vs A-B-C");

    // Calculated A-(B-C) distance-angle map
    auto *dAngleBC = dAngleBCGraph_->createRenderable(
        Renderable::Data2DRenderable, fmt::format("{}//Process2D//{}//DAngle(A-(B-C))", module_->uniqueName(), cfg->niceName()),
        "B-C vs A-B-C");
}

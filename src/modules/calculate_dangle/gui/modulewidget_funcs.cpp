// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "modules/calculate_dangle/dangle.h"
#include "modules/calculate_dangle/gui/modulewidget.h"

CalculateDAngleModuleWidget::CalculateDAngleModuleWidget(QWidget *parent, CalculateDAngleModule *module)
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

    // Set up Distance-Angle graph
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
void CalculateDAngleModuleWidget::updateControls(int flags)
{
    ui_.RDFPlotWidget->updateToolbar();
    ui_.AnglePlotWidget->updateToolbar();
    ui_.DAnglePlotWidget->updateToolbar();

    rdfGraph_->postRedisplay();
    angleGraph_->postRedisplay();
    dAngleGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateDAngleModuleWidget::setGraphDataTargets(CalculateDAngleModule *module)
{
    // Remove any current data
    rdfGraph_->clearRenderables();

    // Loop over Configuration targets in Module
    for (Configuration *cfg : module_->targetConfigurations())
    {
        // Calculated B...C RDF
        auto *rdf = rdfGraph_->createRenderable(
            Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//RDF(BC)", module_->uniqueName(), cfg->niceName()),
            "B...C g(r)");
        rdf->setColour(StockColours::BlueStockColour);

        // Calculated angle histogram
        auto *angle = angleGraph_->createRenderable(
            Renderable::Data1DRenderable, fmt::format("{}//Process1D//{}//Angle(ABC)", module_->uniqueName(), cfg->niceName()),
            "A-B...C Angle");
        angle->setColour(StockColours::RedStockColour);

        // Calculated distance-angle map
        dAngleGraph_->createRenderable(Renderable::Data2DRenderable,
                                       fmt::format("{}//Process2D//{}//DAngle(A-BC)", module_->uniqueName(), cfg->niceName()),
                                       "B...C vs A-B...C");
    }
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "gui/render/renderableData2D.h"
#include "modules/angle/angle.h"
#include "modules/angle/gui/angleWidget.h"

AngleModuleWidget::AngleModuleWidget(QWidget *parent, AngleModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
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
    dAngleABView.axes().setTitle(2, "g\\sub{AB}(r)");
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
    dAngleBCView.axes().setTitle(2, "g\\sub{BC}(r)");
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
void AngleModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
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
 * Widgets / Functions
 */

// Set data targets in graphs
void AngleModuleWidget::setGraphDataTargets(AngleModule *module)
{
    // Remove any current data
    rdfABGraph_->clearRenderables();
    rdfBCGraph_->clearRenderables();
    angleGraph_->clearRenderables();
    dAngleABGraph_->clearRenderables();
    dAngleBCGraph_->clearRenderables();

    // Get Configuration target
    auto *cfg = module_->keywords().getConfiguration("Configuration");
    if (!cfg)
        return;

    // Calculated A...B RDF
    auto rdfAB =
        rdfABGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//RDF(AB)", module_->name()), "A...B g(r)");
    rdfAB->setColour(StockColours::BlueStockColour);

    // Calculated B...C RDF
    auto rdfBC =
        rdfBCGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//RDF(BC)", module_->name()), "B...C g(r)");
    rdfBC->setColour(StockColours::BlueStockColour);

    // Calculated angle histogram
    auto angle = angleGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//Angle(ABC)", module_->name()),
                                                                 "A-B...C Angle");
    angle->setColour(StockColours::RedStockColour);

    // Calculated (A-B)-C distance-angle map
    dAngleABGraph_->createRenderable<RenderableData2D>(fmt::format("{}//Process2D//DAngle((A-B)-C)", module_->name()),
                                                       "A-B vs A-B-C");

    // Calculated A-(B-C) distance-angle map
    dAngleBCGraph_->createRenderable<RenderableData2D>(fmt::format("{}//Process2D//DAngle(A-(B-C))", module_->name()),
                                                       "B-C vs A-B-C");
}

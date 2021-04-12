// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderabledata1d.h"
#include "gui/render/renderabledata2d.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_axisangle/gui/modulewidget.h"

CalculateAxisAngleModuleWidget::CalculateAxisAngleModuleWidget(QWidget *parent, const GenericList &processingData,
                                                               CalculateAxisAngleModule *module)
    : ModuleWidget(parent, processingData), module_(module)
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

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void CalculateAxisAngleModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables)
        rdfGraph_->clearRenderables();

    // Calculated A...B RDF
    if (rdfGraph_->renderables().empty())
        for (const auto *cfg : module_->targetConfigurations())
            rdfGraph_
                ->createRenderable<RenderableData1D>(
                    fmt::format("{}//Process1D//RDF(AB)", module_->uniqueName(), cfg->niceName()), "A...B g(r)")
                ->setColour(StockColours::BlueStockColour);

    // Calculated angle histogram
    if (angleGraph_->renderables().empty())
        for (const auto *cfg : module_->targetConfigurations())
            angleGraph_
                ->createRenderable<RenderableData1D>(
                    fmt::format("{}//Process1D//AxisAngle(AB)", module_->uniqueName(), cfg->niceName()), "Axis Angle")
                ->setColour(StockColours::RedStockColour);

    // Calculated distance-angle map
    if (dAngleGraph_->renderables().empty())
        for (const auto *cfg : module_->targetConfigurations())
            dAngleGraph_->createRenderable<RenderableData2D>(
                fmt::format("{}//Process2D//DAxisAngle", module_->uniqueName(), cfg->niceName()), "A...B vs Axis Angle");

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(processingData_);
    angleGraph_->validateRenderables(processingData_);
    dAngleGraph_->validateRenderables(processingData_);

    ui_.RDFPlotWidget->updateToolbar();
    ui_.AnglePlotWidget->updateToolbar();
    ui_.DAnglePlotWidget->updateToolbar();

    rdfGraph_->postRedisplay();
    angleGraph_->postRedisplay();
    dAngleGraph_->postRedisplay();
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderabledata1d.h"
#include "gui/render/renderabledata2d.h"
#include "modules/intraangle/gui/intraanglewidget.h"
#include "modules/intraangle/intraangle.h"

IntraAngleModuleWidget::IntraAngleModuleWidget(QWidget *parent, IntraAngleModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up Angle graph
    angleGraph_ = ui_.AnglePlotWidget->dataViewer();

    View &angleView = angleGraph_->view();
    angleView.setViewType(View::FlatXYView);
    angleView.axes().setTitle(0, "Angle, \\sym{degree}");
    angleView.axes().setRange(0, 0.0, 180.0);
    angleView.axes().setTitle(1, "Normalised Population");
    angleView.axes().setRange(1, 0.0, 5.0);
    angleView.setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void IntraAngleModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    ui_.AnglePlotWidget->updateToolbar();
    angleGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void IntraAngleModuleWidget::setGraphDataTargets(IntraAngleModule *module)
{
    // Remove any current data
    angleGraph_->clearRenderables();

    // Get Configuration target
    auto *cfg = module_->keywords().getConfiguration("Configuration");
    if (!cfg)
        return;

    // Calculated angle histogram
    auto angle = angleGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//Angle(ABC)", module_->name()),
                                                                 "A-B...C Angle");
    angle->setColour(StockColours::RedStockColour);
}

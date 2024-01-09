// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "gui/render/renderableData2D.h"
#include "modules/intraAngle/gui/intraAngleWidget.h"
#include "modules/intraAngle/intraAngle.h"

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

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void IntraAngleModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        angleGraph_->clearRenderables();

    // Calculated angle histogram
    if (angleGraph_->renderables().empty())
        angleGraph_
            ->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//Angle(ABC)", module_->name()), "A-B...C Angle")
            ->setColour(StockColours::RedStockColour);

    ui_.AnglePlotWidget->updateToolbar();
    angleGraph_->postRedisplay();
}

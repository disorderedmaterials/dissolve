// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "math/integrator.h"
#include "modules/ringPopulations/gui/ringPopulationsWidget.h"
#include "modules/ringPopulations/ringPopulations.h"

RingPopulationsModuleWidget::RingPopulationsModuleWidget(QWidget *parent, RingPopulationsModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    ringPopulationsGraph_ = ui_.RingPopulationsPlotWidget->dataViewer();

    auto &view = ringPopulationsGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Qn");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void RingPopulationsModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        ringPopulationsGraph_->clearRenderables();

    if (ringPopulationsGraph_->renderables().empty())
        ringPopulationsGraph_->createRenderable<RenderableData1D>(fmt::format("{}//QSpecies", module_->name()), "Q-Species");

    // Validate renderables if they need it
    ringPopulationsGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.RingPopulationsPlotWidget->updateToolbar();

    ringPopulationsGraph_->postRedisplay();
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/intraDistance/gui/intraDistanceWidget.h"
#include "modules/intraDistance/intraDistance.h"

IntraDistanceModuleWidget::IntraDistanceModuleWidget(QWidget *parent, IntraDistanceModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget->dataViewer();

    auto &view = rdfGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void IntraDistanceModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        rdfGraph_->clearRenderables();

    if (rdfGraph_->renderables().empty())
    {
        auto *cfg = module_->keywords().getConfiguration("Configuration");
        if (cfg)
            rdfGraph_
                ->createRenderable<RenderableData1D>(fmt::format("{}//NormalisedHistogram", module_->name()),
                                                     fmt::format("RDF//{}", cfg->niceName()), cfg->niceName())
                ->setColour(StockColours::BlueStockColour);
    }

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.RDFPlotWidget->updateToolbar();
    rdfGraph_->postRedisplay();
}

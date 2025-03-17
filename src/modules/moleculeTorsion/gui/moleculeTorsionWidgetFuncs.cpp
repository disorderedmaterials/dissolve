// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/moleculeTorsion/gui/moleculeTorsionWidget.h"
#include "modules/moleculeTorsion/moleculeTorsion.h"

MoleculeTorsionModuleWidget::MoleculeTorsionModuleWidget(QWidget *parent, MoleculeTorsionModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    histogramGraph_ = ui_.HistogramPlotWidget->dataViewer();

    auto &view = histogramGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Torsion, \\sym{degree}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void MoleculeTorsionModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        histogramGraph_->clearRenderables();

    if (histogramGraph_->renderables().empty())
    {
        auto *cfg = module_->keywords().getConfiguration("Configuration");
        if (cfg)
            histogramGraph_
                ->createRenderable<RenderableData1D>(std::format("{}//NormalisedHistogram", module_->name()),
                                                     std::format("RDF//{}", cfg->niceName()), cfg->niceName())
                ->setColour(StockColours::BlueStockColour);
    }

    // Validate renderables if they need it
    histogramGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.HistogramPlotWidget->updateToolbar();
    histogramGraph_->postRedisplay();
}

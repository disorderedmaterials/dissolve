// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/siteRDF/gui/siteRDFWidget.h"
#include "modules/siteRDF/siteRDF.h"

SiteRDFModuleWidget::SiteRDFModuleWidget(QWidget *parent, SiteRDFModule *module, Dissolve &dissolve)
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
    view.axes().setTitle(1, "g(r)");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    // Set up RDF graph
    runningCNGraph_ = ui_.RunningCNPlotWidget->dataViewer();

    auto &runningCNNview = runningCNGraph_->view();
    runningCNNview.setViewType(View::FlatXYView);
    runningCNNview.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    runningCNNview.axes().setMax(0, 10.0);
    runningCNNview.axes().setTitle(1, "Coordination Number");
    runningCNNview.axes().setMin(1, 0.0);
    runningCNNview.axes().setMax(1, 1.0);
    runningCNNview.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void SiteRDFModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    // Update CN labels
    auto rangeAOn = module_->isRangeEnabled(0);
    ui_.RegionAResultFrame->setText(
        rangeAOn ? dissolve_.processingModuleData().valueOr("CN//A", module_->name(), SampledDouble()) : SampledDouble());
    auto rangeBOn = module_->isRangeEnabled(1);
    ui_.RegionBResultFrame->setText(
        rangeBOn ? dissolve_.processingModuleData().valueOr("CN//B", module_->name(), SampledDouble()) : SampledDouble());
    ui_.RegionBResultFrame->setEnabled(rangeBOn);
    auto rangeCOn = module_->isRangeEnabled(2);
    ui_.RegionCResultFrame->setText(
        rangeCOn ? dissolve_.processingModuleData().valueOr("CN//C", module_->name(), SampledDouble()) : SampledDouble());
    ui_.RegionCResultFrame->setEnabled(rangeCOn);

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        rdfGraph_->clearRenderables();
        runningCNGraph_->clearRenderables();
    }

    if (rdfGraph_->renderables().empty())
    {
        auto *cfg = module_->keywords().getConfiguration("Configuration");
        if (cfg)
            rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//RDF", module_->name()),
                                                          fmt::format("RDF//{}", cfg->niceName()), cfg->niceName());
    }
    if (runningCNGraph_->renderables().empty())
        runningCNGraph_->createRenderable<RenderableData1D>(fmt::format("{}//RunningCN", module_->name()), "Running CN");

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(dissolve_.processingModuleData());
    runningCNGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.RDFPlotWidget->updateToolbar();
    ui_.RunningCNPlotWidget->updateToolbar();
    rdfGraph_->postRedisplay();
    runningCNGraph_->postRedisplay();
}

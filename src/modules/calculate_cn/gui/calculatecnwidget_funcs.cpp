// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/render/renderabledata1d.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_cn/gui/calculatecnwidget.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"

CalculateCNModuleWidget::CalculateCNModuleWidget(QWidget *parent, CalculateCNModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget;

    auto &view = rdfGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "g(r)");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    rdfDataLocated_ = false;

    refreshing_ = false;
}

// Update controls within widget
void CalculateCNModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    // Update CN labels
    ui_.RegionAResultFrame->setText(
        dissolve_.processingModuleData().valueOr("Analyser//Sum1D//CN//A", module_->uniqueName(), SampledDouble()));
    auto rangeBOn = module_->isRangeBEnabled();
    ui_.RegionBResultFrame->setText(
        rangeBOn ? dissolve_.processingModuleData().valueOr("Analyser//Sum1D//CN//B", module_->uniqueName(), SampledDouble())
                 : SampledDouble());
    ui_.RegionBResultFrame->setEnabled(rangeBOn);
    auto rangeCOn = module_->isRangeCEnabled();
    ui_.RegionCResultFrame->setText(
        rangeCOn ? dissolve_.processingModuleData().valueOr("Analyser//Sum1D//CN//C", module_->uniqueName(), SampledDouble())
                 : SampledDouble());
    ui_.RegionCResultFrame->setEnabled(rangeCOn);

    // Clear and recreate graph data targets?
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || !rdfData_)
    {
        rdfGraph_->clearRenderables();

        // Get target RDF module
        auto optRDFModule = module_->keywords().get<const CalculateRDFModule *, ModuleKeyword<CalculateRDFModule>>("SourceRDF");
        if (!optRDFModule)
            return;

        const auto *rdfModule = optRDFModule->get();
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//RDF", rdfModule->uniqueName()), "RDF");
    }

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(dissolve_.processingModuleData());

    rdfGraph_->postRedisplay();
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/renderabledata1d.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_cn/gui/modulewidget.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/process1d.h"

CalculateCNModuleWidget::CalculateCNModuleWidget(QWidget *parent, const GenericList &processingData,
                                                 CalculateCNModule *cnModule)
    : ModuleWidget(parent, processingData), module_(cnModule)
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
void CalculateCNModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    // Update CN labels
    ui_.RegionAResultFrame->setText(module_->coordinationNumber(0));
    auto rangeBOn = module_->isRangeBEnabled();
    ui_.RegionBResultFrame->setText(rangeBOn ? module_->coordinationNumber(1) : SampledDouble());
    ui_.RegionBResultFrame->setEnabled(rangeBOn);
    auto rangeCOn = module_->isRangeCEnabled();
    ui_.RegionCResultFrame->setText(rangeCOn ? module_->coordinationNumber(2) : SampledDouble());
    ui_.RegionCResultFrame->setEnabled(rangeCOn);

    // Clear and recreate graph data targets?
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables || !rdfData_)
    {
        rdfGraph_->clearRenderables();

        // Get target RDF module
        auto found = false;
        auto *rdfModule = module_->keywords().retrieve<const CalculateRDFModule *>("SourceRDF", nullptr, &found);
        if (!rdfModule)
            return;

        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Process1D//RDF", rdfModule->uniqueName()), "RDF");
    }

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(processingData_);

    rdfGraph_->postRedisplay();
}

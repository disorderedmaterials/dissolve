// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/partialsetaccumulator.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "main/dissolve.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/modulewidget.h"
#include "templates/algorithms.h"

AccumulateModuleWidget::AccumulateModuleWidget(QWidget *parent, const GenericList &processingData, AccumulateModule *module,
                                               Dissolve &dissolve)
    : ModuleWidget(parent, processingData), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up S(Q) graph
    graph_ = ui_.PlotWidget->dataViewer();
    auto hasSQ =
        module_->keywords().enumeration<AccumulateModule::TargetPartialSet>("Data") == AccumulateModule::TargetPartialSet::SQ;

    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setTitle(0, hasSQ ? "\\it{Q}, \\sym{angstrom}\\sup{-1}" : "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setTitle(1, hasSQ ? "F(Q)" : "G(r)");
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    graph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    graph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

    refreshing_ = false;
}

AccumulateModuleWidget::~AccumulateModuleWidget() {}

/*
 * UI
 */

// Create renderables for current target PartialSet
void AccumulateModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSetAccumulator &ps = *targetPartials_;

    for (auto &&[full, bound, unbound] : zip(ps.partials(), ps.boundPartials(), ps.unboundPartials()))
    {
        // Get atom type pair id from the full partial tag
        auto id = DissolveSys::beforeChar(full.tag(), '/');

        // Full partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}", module_->uniqueName(), targetPrefix, full.tag()),
                                                   fmt::format("{} (Full)", id), "Full");

        // Bound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}", module_->uniqueName(), targetPrefix, bound.tag()),
                                                   fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        graph_->createRenderable<RenderableData1D>(
            fmt::format("{}//{}//{}", module_->uniqueName(), targetPrefix, unbound.tag()), fmt::format("{} (Unbound)", id),
            "Unbound");
    }
}

// Update controls within widget
void AccumulateModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    // Set button texts
    auto hasSQ =
        module_->keywords().enumeration<AccumulateModule::TargetPartialSet>("Data") == AccumulateModule::TargetPartialSet::SQ;
    ui_.TotalButton->setText(hasSQ ? "Total F(Q)" : "Total G(r)");
    ui_.PartialsButton->setText(hasSQ ? "Partial S(Q)" : "Partial g(r)");

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables || (!ui_.TotalButton->isChecked() && !targetPartials_))
    {
        ui_.PlotWidget->clearRenderableData();

        if (ui_.PartialsButton->isChecked())
        {
            targetPartials_ = processingData_.valueIf<PartialSetAccumulator>("Accumulation", module_->uniqueName());
            createPartialSetRenderables("Accumulation");
        }
        else
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//Accumulation//Total", module_->uniqueName()), "Total",
                                                       "Calc");
    }

    // Validate renderables if they need it
    graph_->validateRenderables(processingData_);

    graph_->postRedisplay();
    ui_.PlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void AccumulateModuleWidget::on_TotalButton_clicked(bool checked)
{
    if (!checked)
        return;

    auto hasSQ =
        module_->keywords().enumeration<AccumulateModule::TargetPartialSet>("Data") == AccumulateModule::TargetPartialSet::SQ;

    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    graph_->view().axes().setTitle(0, hasSQ ? "\\it{Q}, \\sym{angstrom}\\sup{-1}" : "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, hasSQ ? "F(Q)" : "G(r)");

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void AccumulateModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    auto hasSQ =
        module_->keywords().enumeration<AccumulateModule::TargetPartialSet>("Data") == AccumulateModule::TargetPartialSet::SQ;

    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().axes().setTitle(0, hasSQ ? "\\it{Q}, \\sym{angstrom}\\sup{-1}" : "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, hasSQ ? "S(Q)" : "g(r)");

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

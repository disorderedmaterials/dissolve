// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/sq/gui/modulewidget.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

SQModuleWidget::SQModuleWidget(QWidget *parent, const GenericList &processingData, SQModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, processingData), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up S(Q) graph

    sqGraph_ = ui_.SQPlotWidget->dataViewer();

    sqGraph_->view().setViewType(View::FlatXYView);
    sqGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    sqGraph_->view().axes().setMax(0, 10.0);
    sqGraph_->view().axes().setTitle(1, "F(Q)");
    sqGraph_->view().axes().setMin(1, -1.0);
    sqGraph_->view().axes().setMax(1, 1.0);
    sqGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    sqGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    sqGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    sqGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    sqGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    sqGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    sqGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    sqGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    sqGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    sqGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

    refreshing_ = false;
}

SQModuleWidget::~SQModuleWidget() {}

/*
 * UI
 */

// Create renderables for current target PartialSet
void SQModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    for_each_pair(ps.atomTypes().begin(), ps.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

        // Full partial
        sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->uniqueName(), targetPrefix, id),
                                                     fmt::format("{} (Full)", id), "Full");

        // Bound partial
        sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->uniqueName(), targetPrefix, id),
                                                     fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        sqGraph_->createRenderable<RenderableData1D>(
            fmt::format("{}//{}//{}//Unbound", module_->uniqueName(), targetPrefix, id), fmt::format("{} (Unbound)", id),
            "Unbound");
    });
}

// Update controls within widget
void SQModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables || (!ui_.TotalButton->isChecked() && !targetPartials_))
    {
        ui_.SQPlotWidget->clearRenderableData();

        if (ui_.PartialsButton->isChecked())
        {
            targetPartials_ = processingData_.valueIf<PartialSet>("UnweightedSQ", module_->uniqueName());
            createPartialSetRenderables("UnweightedSQ");
        }
        else
            sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//UnweightedSQ//Total", module_->uniqueName()), "Total",
                                                         "Calc");
    }

    // Validate renderables if they need it
    sqGraph_->validateRenderables(processingData_);

    sqGraph_->postRedisplay();
    ui_.SQPlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void SQModuleWidget::on_TotalButton_clicked(bool checked)
{
    if (!checked)
        return;

    sqGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    sqGraph_->view().axes().setTitle(1, "F(Q)");

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void SQModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    sqGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    sqGraph_->view().axes().setTitle(1, "S(Q)");

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

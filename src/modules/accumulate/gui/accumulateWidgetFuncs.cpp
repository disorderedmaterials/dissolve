// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/partialSetAccumulator.h"
#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/accumulate/accumulate.h"
#include "modules/accumulate/gui/accumulateWidget.h"
#include "templates/algorithms.h"

AccumulateModuleWidget::AccumulateModuleWidget(QWidget *parent, AccumulateModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set model for module selector
    ui_.ModuleTargetCombo->setModel(&moduleModel_);

    // Set up S(Q) graph
    graph_ = ui_.PlotWidget->dataViewer();
    auto hasSQ = module_->keywords().getEnumeration<AccumulateModule::TargetPartialSet>("Data") ==
                 AccumulateModule::TargetPartialSet::SQ;

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

/*
 * UI
 */

// Create renderables for current target PartialSet
void AccumulateModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSetAccumulator &ps = *targetPartials_;

    // Get the filter text (if there is any)
    std::optional<std::string> filterText;
    if (!ui_.FilterEdit->text().isEmpty())
        filterText = ui_.FilterEdit->text().toStdString();

    for (auto &&[full, bound, unbound] : zip(ps.partials(), ps.boundPartials(), ps.unboundPartials()))
    {
        // Get atom type pair id from the full partial tag
        auto id = DissolveSys::beforeChar(full.tag(), '/');

        // Filtering - does this 'id' match our filter?
        if (filterText && id.find(filterText.value()) == std::string::npos)
            continue;

        // Full partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}", module_->name(), targetPrefix, full.tag()),
                                                   fmt::format("{} (Full)", id), "Full");

        // Bound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}", module_->name(), targetPrefix, bound.tag()),
                                                   fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}", module_->name(), targetPrefix, unbound.tag()),
                                                   fmt::format("{} (Unbound)", id), "Unbound");
    }
}

// Update controls within widget
void AccumulateModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Update model
    auto &moduleTargets = module_->keywords().getVectorModule("Targets");
    moduleModel_.setData(moduleTargets);

    // Find the previously selected module in the vector
    auto moduleIt = std::find(moduleTargets.begin(), moduleTargets.end(), currentTargetModule_);
    if (moduleIt == moduleTargets.end())
        currentTargetModule_ = ui_.ModuleTargetCombo->currentData().value<Module *>();
    else
        ui_.ModuleTargetCombo->setCurrentIndex(moduleIt - moduleTargets.begin());

    if (!currentTargetModule_)
    {
        targetPartials_ = std::nullopt;
        ui_.PlotWidget->clearRenderableData();
    }
    else
    {
        // Set button texts
        auto hasSQ = module_->keywords().getEnumeration<AccumulateModule::TargetPartialSet>("Data") ==
                     AccumulateModule::TargetPartialSet::SQ;
        ui_.TotalButton->setText(hasSQ ? "Total F(Q)" : "Total G(r)");
        ui_.PartialsButton->setText(hasSQ ? "Partial S(Q)" : "Partial g(r)");

        // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet
        if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || (!ui_.TotalButton->isChecked() && !targetPartials_))
        {
            ui_.PlotWidget->clearRenderableData();

            if (ui_.PartialsButton->isChecked())
            {
                targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSetAccumulator>(currentTargetModule_->name(),
                                                                                                  module_->name());
                createPartialSetRenderables(currentTargetModule_->name());
            }
            else
                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//{}//Total", module_->name(), currentTargetModule_->name()), "Total", "Calc");
        }
    }

    // Validate renderables if they need it
    graph_->validateRenderables(dissolve_.processingModuleData());

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

    auto hasSQ = module_->keywords().getEnumeration<AccumulateModule::TargetPartialSet>("Data") ==
                 AccumulateModule::TargetPartialSet::SQ;

    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    graph_->view().axes().setTitle(0, hasSQ ? "\\it{Q}, \\sym{angstrom}\\sup{-1}" : "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, hasSQ ? "F(Q)" : "G(r)");

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void AccumulateModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    auto hasSQ = module_->keywords().getEnumeration<AccumulateModule::TargetPartialSet>("Data") ==
                 AccumulateModule::TargetPartialSet::SQ;

    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().axes().setTitle(0, hasSQ ? "\\it{Q}, \\sym{angstrom}\\sup{-1}" : "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, hasSQ ? "S(Q)" : "g(r)");

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void AccumulateModuleWidget::on_ModuleTargetCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    currentTargetModule_ = ui_.ModuleTargetCombo->currentData().value<Module *>();

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void AccumulateModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void AccumulateModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->clear();

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

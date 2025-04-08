// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"
#include "modules/epsrManager/gui/epsrManagerWidget.h"

EPSRManagerModuleWidget::EPSRManagerModuleWidget(QWidget *parent, EPSRManagerModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up the main graph
    graph_ = ui_.PlotWidget->dataViewer();
    // -- Set view
    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    graph_->groupManager().setGroupColouring("Phi", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Phi", RenderableGroup::PreventVerticalShifting);
    graph_->groupManager().setGroupColouring("RFactor", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void EPSRManagerModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        ui_.PlotWidget->clearRenderableData();

        const auto &atomTypes = dissolve_.coreData().atomTypes();
        const auto nAtomTypes = atomTypes.size();

        if (ui_.PotentialsButton->isChecked())
        {
            // Add on additional potentials
            dissolve::for_each_pair(ParallelPolicies::seq, atomTypes.begin(), atomTypes.end(),
                                    [&](int typeI, const auto &at1, int typeJ, const auto &at2)
                                    {
                                        const std::string id = std::format("{}-{}", at1->name(), at2->name());

                                        auto pp = dissolve_.pairPotential(at1, at2);
                                        if (pp)
                                            graph_->createRenderable<RenderableData1D>(
                                                std::format("Dissolve//Potential_{}_Additional", id), id, "Phi");
                                    });
        }
        if (ui_.RFactorButton->isChecked())
        {
            graph_->groupManager().removeVerticalShifts();
            auto epsrModuleTargets = module_->keywords().getVectorModule("Target");
            for (auto *targetModule : epsrModuleTargets)
                graph_->createRenderable<RenderableData1D>(std::format("{}//RFactor", targetModule->name()),
                                                           targetModule->name(), "RFactor");
        }
    }

    auto rFactorArray = dissolve_.processingModuleData().valueIf<Data1D>("RFactor", module_->name());

    // Validate renderables if they need it
    graph_->validateRenderables(dissolve_.processingModuleData());

    ui_.PlotWidget->updateToolbar();

    graph_->postRedisplay();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void EPSRManagerModuleWidget::on_PotentialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRManagerModuleWidget::on_RFactorButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "Iteration");
    graph_->view().axes().setTitle(1, "R-Factor");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::XAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Integer));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Scientific));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

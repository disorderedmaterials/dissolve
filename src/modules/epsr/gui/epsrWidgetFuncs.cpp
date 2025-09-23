// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomType.h"
#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "module/group.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/epsrWidget.h"
#include "modules/gr/gr.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

EPSRModuleWidget::EPSRModuleWidget(QWidget *parent, EPSRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up the main graph
    graph_ = ui_.PlotWidget->dataViewer();

    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setTitle(1, "F(\\it{Q})");
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    graph_->groupManager().setGroupColouring("Fit", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Fit", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupStipple("Fit", LineStipple::QuarterDashStipple);
    graph_->groupManager().setGroupColouring("Estimated", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Estimated", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Calc", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupStipple("Calc", LineStipple::QuarterDashStipple);
    graph_->groupManager().setGroupColouring("Delta", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Delta", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupStipple("Delta", LineStipple::DotStipple);
    graph_->groupManager().setGroupColouring("Exp", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Exp", RenderableGroup::IndividualVerticalShifting);
    graph_->groupManager().setGroupColouring("Phi", RenderableGroup::AutomaticIndividualColouring);
    graph_->groupManager().setGroupVerticalShifting("Phi", RenderableGroup::PreventVerticalShifting);
    graph_->groupManager().setGroupColouring("RFactor", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void EPSRModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        ui_.PlotWidget->clearRenderableData();

        auto epsrModuleTargets = module_->keywords().getVectorModule("Target");

        // Set the relevant graph targets
        if (ui_.TotalFQButton->isChecked())
        {
            // Add calculated and reference F(Q) for all targets
            for (auto *targetModule : epsrModuleTargets)
            {
                // Reference data
                graph_->createRenderable<RenderableData1D>(std::format("{}//ReferenceData", targetModule->name()),
                                                           std::format("{} (Exp)", targetModule->name()), "Exp");

                // Calculated F(Q)
                graph_->createRenderable<RenderableData1D>(std::format("{}//WeightedSQ//Total", targetModule->name()),
                                                           std::format("{} (Calc)", targetModule->name()), "Calc");

                // F(Q) diff w.r.t. reference
                graph_->createRenderable<RenderableData1D>(
                    std::format("{}//Difference//{}", module_->name(), targetModule->name()),
                    std::format("{} (Delta)", targetModule->name()), "Delta");
            }
        }
        else if (ui_.DeltaFQButton->isChecked())
        {
            // Add delta F(Q) and fit
            for (auto *targetModule : epsrModuleTargets)
            {
                graph_->createRenderable<RenderableData1D>(
                    std::format("{}//DeltaFQ//{}", module_->name(), targetModule->name()),
                    std::format("{} (Delta)", targetModule->name()), "Delta");

                graph_->createRenderable<RenderableData1D>(
                    std::format("{}//DeltaFQFit//{}", module_->name(), targetModule->name()),
                    std::format("{} (Fit)", targetModule->name()), "Fit");
            }
        }
        else if (ui_.EstimatedSQButton->isChecked())
        {
            auto optEstimatedSQ =
                dissolve_.processingModuleData().valueIf<DoubleKeyedMap<Data1D>>("EstimatedSQ", module_->name());
            if (optEstimatedSQ)
            {
                auto &estimatedSQ = optEstimatedSQ->get();
                for (auto &[key, value] : estimatedSQ)
                {
                    // Get constituent key pair and reformat to get the tag
                    auto keyPair = estimatedSQ.keyPair(key);
                    const auto keyString = std::format("{}-{}", keyPair.first, keyPair.second);

                    // Unweighted estimated partial
                    graph_->createRenderable<RenderableData1D>(std::format("{}//EstimatedSQ//{}", module_->name(), keyString),
                                                               std::format("{} (Estimated)", keyString), "Estimated");

                    // Calculated / summed partial
                    graph_->createRenderable<RenderableData1D>(std::format("{}//UnweightedSQ//{}", module_->name(), keyString),
                                                               std::format("{} (Calc)", keyString), "Calc");

                    // Deltas
                    graph_->createRenderable<RenderableData1D>(std::format("{}//DeltaSQ//{}", module_->name(), keyString),
                                                               std::format("{} (Delta)", keyString), "Delta");
                }
            }
        }
        else if (ui_.EstimatedGRButton->isChecked())
        {
            // Get underlying RDF module
            std::string rdfModuleName = "UNKNOWN_RDF_MODULE";
            const auto targets = module_->keywords().getVectorModule("Target");
            if (!targets.empty())
            {
                auto optSQModule = targets[0]->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
                if (!optSQModule)
                    Messenger::error("Couldn't get any S(Q) data from the first target module, so underlying partial g(r) will "
                                     "be unavailable.",
                                     module_->name());
                else
                {
                    auto optRDFModule =
                        optSQModule.value()->keywords().get<const GRModule *, ModuleKeyword<const GRModule>>("SourceRDFs");
                    if (optRDFModule)
                        rdfModuleName = optRDFModule.value()->name();
                    else
                        rdfModuleName = "UNKNOWN_RDF_MODULE";
                }
            }

            // Add experimentally-determined partial g(r)
            auto optEstimatedGR =
                dissolve_.processingModuleData().valueIf<DoubleKeyedMap<Data1D>>("EstimatedGR", module_->name());
            if (optEstimatedGR)
            {
                auto &estimatedGR = optEstimatedGR->get();
                for (auto &[key, value] : estimatedGR)
                {
                    // Get constituent key pair and reformat to get the tag
                    auto keyPair = estimatedGR.keyPair(key);
                    const auto keyString = std::format("{}-{}", keyPair.first, keyPair.second);

                    // Experimentally-determined unweighted partial
                    graph_->createRenderable<RenderableData1D>(std::format("{}//EstimatedGR//{}", module_->name(), keyString),
                                                               std::format("{} (Estimated)", keyString), "Estimated");

                    // Calculated / summed partials, taken from the RDF module referenced by the first module target
                    graph_->createRenderable<RenderableData1D>(
                        std::format("{}//UnweightedGR//{}//Full", rdfModuleName, keyString),
                        std::format("{} (Calc)", keyString), "Calc");
                }
            }
        }
        else if (ui_.TotalGRButton->isChecked())
        {
            for (auto *targetModule : epsrModuleTargets)
            {
                // Reference F(r) (from direct FT of input data)
                graph_->createRenderable<RenderableData1D>(std::format("{}//ReferenceDataFT", targetModule->name()),
                                                           std::format("{} (Exp)", targetModule->name()), "Exp");

                // Simulated F(r) (from FT of the calculated F(Q))
                graph_->createRenderable<RenderableData1D>(
                    std::format("{}//SimulatedFR//{}", module_->name(), targetModule->name()),
                    std::format("{} (Calc)", targetModule->name()), "Calc");
            }
        }
        else if (ui_.PotentialsButton->isChecked())
        {
            // Add on additional potentials - use the EstimatedSQ data as the key reference
            auto optEstimatedSQ =
                dissolve_.processingModuleData().valueIf<DoubleKeyedMap<Data1D>>("EstimatedSQ", module_->name());
            if (optEstimatedSQ)
            {
                auto &estimatedSQ = optEstimatedSQ->get();
                for (auto &[key, value] : estimatedSQ)
                {
                    // Get constituent key pair and reformat to get the tag
                    auto keyPair = estimatedSQ.keyPair(key);
                    const auto keyString = std::format("{}-{}", keyPair.first, keyPair.second);
                    graph_->createRenderable<RenderableData1D>(std::format("Dissolve//Potential_{}_Additional", keyString),
                                                               keyString, "Phi");
                }
            }
        }
        else if (ui_.RFactorButton->isChecked())
        {
            graph_->groupManager().removeVerticalShifts();

            // Add total R-factor followed by those for each target
            graph_->createRenderable<RenderableData1D>(std::format("{}//RFactor", module_->name()), "Total", "Total")
                ->lineStyle()
                .setStipple(LineStipple::HalfDashStipple);

            for (auto *targetModule : epsrModuleTargets)
                graph_->createRenderable<RenderableData1D>(
                    std::format("{}//RFactor//{}", module_->name(), targetModule->name()), targetModule->name(), "RFactor");
        }
        else if (ui_.EReqButton->isChecked())
        {
            // Add phi magnitude data
            graph_->createRenderable<RenderableData1D>(std::format("{}//EPMag", module_->name()), "EReq", "EReq");
        }
    }

    // Set information labels
    auto eReqArray = dissolve_.processingModuleData().valueIf<Data1D>("EPMag", module_->name());
    ui_.EReqValueLabel->setText(eReqArray ? QString::number(eReqArray->get().values().back()) : "--");
    auto rFactorArray = dissolve_.processingModuleData().valueIf<Data1D>("RFactor", module_->name());
    ui_.RFactorValueLabel->setText(rFactorArray ? QString::number(rFactorArray->get().values().back()) : "--");

    // Validate renderables if they need it
    graph_->validateRenderables(dissolve_.processingModuleData());

    ui_.PlotWidget->updateToolbar();

    graph_->postRedisplay();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void EPSRModuleWidget::on_TotalFQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "F(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRModuleWidget::on_DeltaFQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "\\sym{Delta}F(\\it{Q})");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRModuleWidget::on_EstimatedSQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "Estimated S(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRModuleWidget::on_EstimatedGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "Estimated g(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRModuleWidget::on_TotalGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "G(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Decimal));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void EPSRModuleWidget::on_PotentialsButton_clicked(bool checked)
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

void EPSRModuleWidget::on_RFactorButton_clicked(bool checked)
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

void EPSRModuleWidget::on_EReqButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "Iteration");
    graph_->view().axes().setTitle(1, "EReq");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    graph_->view().setAutoFollowType(View::XAutoFollow);
    graph_->view().axes().setNumberFormat(0, NumberFormat(NumberFormat::FormatType::Integer));
    graph_->view().axes().setNumberFormat(1, NumberFormat(NumberFormat::FormatType::Decimal));

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

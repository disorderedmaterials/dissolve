// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "module/group.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/modulewidget.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

EPSRModuleWidget::EPSRModuleWidget(QWidget *parent, const GenericList &processingData, EPSRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, processingData), dissolve_(dissolve), module_(module)
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
    graph_->groupManager().setGroupColouring("RFactor", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;
}

EPSRModuleWidget::~EPSRModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void EPSRModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    if (updateType == ModuleWidget::UpdateType::RecreateRenderables || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        ui_.PlotWidget->clearRenderableData();

        // Set the relevant graph targets
        if (ui_.TotalFQButton->isChecked())
        {
            // Add calculated and reference F(Q) for all targets
            for (auto *targetModule : module_->targets())
            {
                // Reference data
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceData", targetModule->uniqueName()),
                                                           fmt::format("{} (Exp)", targetModule->uniqueName()), "Exp");

                // Calculated F(Q)
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedSQ//Total", targetModule->uniqueName()),
                                                           fmt::format("{} (Calc)", targetModule->uniqueName()), "Calc");

                // F(Q) diff w.r.t. reference
                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//Difference//{}", module_->uniqueName(), targetModule->uniqueName()),
                    fmt::format("{} (Delta)", targetModule->uniqueName()), "Delta");
            }
        }
        else if (ui_.DeltaFQButton->isChecked())
        {
            // Add delta F(Q) and fit
            for (auto *targetModule : module_->targets())
            {
                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//DeltaFQ//{}", module_->uniqueName(), targetModule->uniqueName()),
                    fmt::format("{} (Delta)", targetModule->uniqueName()), "Delta");

                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//DeltaFQFit//{}", module_->uniqueName(), targetModule->uniqueName()),
                    fmt::format("{} (Fit)", targetModule->uniqueName()), "Fit");
            }
        }
        else if (ui_.EstimatedSQButton->isChecked())
        {
            // Add experimentally-determined partial S(Q)
            for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
                const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

                // Unweighted estimated partial
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//EstimatedSQ//{}", module_->uniqueName(), id),
                                                           fmt::format("{} (Estimated)", id), "Estimated");

                // Calculated / summed partial
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//UnweightedSQ//{}", module_->uniqueName(), id),
                                                           fmt::format("{} (Calc)", id), "Calc");

                // Deltas
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//DeltaSQ//{}", module_->uniqueName(), id),
                                                           fmt::format("{} (Delta)", id), "Delta");
            });
        }
        else if (ui_.EstimatedGRButton->isChecked())
        {
            // Get underlying RDF module
            std::string rdfModuleName = "UNKNOWN_RDF_MODULE";
            const auto &targets = module_->keywords().retrieve<std::vector<Module *>>("Target");
            if (!targets.empty())
            {
                const SQModule *sqModule = targets[0]->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
                if (!sqModule)
                    Messenger::error("Couldn't get any S(Q) data from the first target module, so underlying partial g(r) will "
                                     "be unavailable.",
                                     module_->uniqueName());
                else
                {
                    auto *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
                    if (rdfModule)
                        rdfModuleName = rdfModule->uniqueName();
                    else
                        rdfModuleName = "UNKNOWN_RDF_MODULE";
                }
            }

            // Add experimentally-determined partial g(r)
            for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto &at1, int m, auto &at2) {
                const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

                // Experimentally-determined unweighted partial
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//EstimatedGR//{}", module_->uniqueName(), id),
                                                           fmt::format("{} (Estimated)", id), "Estimated");

                // Calculated / summed partials, taken from the RDF module referenced by the first module target
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//UnweightedGR//{}//Full", rdfModuleName, id),
                                                           fmt::format("{} (Calc)", id), "Calc");
            });
        }
        else if (ui_.TotalGRButton->isChecked())
        {
            for (auto *targetModule : module_->targets())
            {
                // Reference F(r) (from direct FT of input data)
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceDataFT", targetModule->uniqueName()),
                                                           fmt::format("{} (Exp)", targetModule->uniqueName()), "Exp");

                // Simulated F(r) (from FT of the calculated F(Q))
                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//SimulatedFR//{}", module_->uniqueName(), targetModule->uniqueName()),
                    fmt::format("{} (Calc)", targetModule->uniqueName()), "Calc");
            }
        }
        else if (ui_.PotentialsButton->isChecked())
        {
            // Add on additional potentials
            for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
                const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

                graph_->createRenderable<RenderableData1D, Data1D>(dissolve_.pairPotential(at1, at2)->uAdditional(), id, "Phi");
            });
        }
        else if (ui_.RFactorButton->isChecked())
        {
            // Add total R-factor followed by those for each target
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//RFactor", module_->uniqueName()), "Total", "Total")
                ->lineStyle()
                .setStipple(LineStipple::HalfDashStipple);

            for (auto *targetModule : module_->targets())
                graph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//RFactor//{}", module_->uniqueName(), targetModule->uniqueName()),
                    targetModule->uniqueName(), "RFactor");
        }
        else if (ui_.EReqButton->isChecked())
        {
            // Add phi magnitude data
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//EPMag", module_->uniqueName()), "EReq", "EReq");
        }
    }

    // Set information labels
    auto eReqArray = processingData_.valueIf<Data1D>("EPMag", module_->uniqueName());
    ui_.EReqValueLabel->setText(eReqArray ? QString::number(eReqArray->get().values().back()) : "--");
    auto rFactorArray = processingData_.valueIf<Data1D>("RFactor", module_->uniqueName());
    ui_.RFactorValueLabel->setText(rFactorArray ? QString::number(rFactorArray->get().values().back()) : "--");

    // Validate renderables if they need it
    graph_->validateRenderables(processingData_);

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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

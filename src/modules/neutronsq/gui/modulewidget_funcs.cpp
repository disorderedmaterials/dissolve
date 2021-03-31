// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/neutronsq/gui/modulewidget.h"
#include "modules/neutronsq/neutronsq.h"
#include "templates/algorithms.h"

NeutronSQModuleWidget::NeutronSQModuleWidget(QWidget *parent, const GenericList &processingData, NeutronSQModule *module,
                                             Dissolve &dissolve)
    : ModuleWidget(parent, processingData), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up graph (defaulting to total F(Q))
    graph_ = ui_.PlotWidget->dataViewer();
    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setTitle(1, "F(Q)");
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

NeutronSQModuleWidget::~NeutronSQModuleWidget() {}

/*
 * UI
 */

// Create renderables for current target PartialSet
void NeutronSQModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    for_each_pair(ps.atomTypes().begin(), ps.atomTypes().end(), [&](int n, auto &at1, int m, auto &at2) {
        const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

        // Full partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->uniqueName(), targetPrefix, id),
                                                   fmt::format("{} (Full)", id), "Full");

        // Bound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->uniqueName(), targetPrefix, id),
                                                   fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Unbound", module_->uniqueName(), targetPrefix, id),
                                                   fmt::format("{} (Unbound)", id), "Unbound");
    });
}

// Update controls within widget
void NeutronSQModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables ||
        (!ui_.TotalFQButton->isChecked() && !ui_.TotalGRButton->isChecked() && !targetPartials_))
    {
        ui_.PlotWidget->clearRenderableData();

        // Grab reference data file and format
        const Data1DImportFileFormat &referenceFileAndFormat = module_->referenceFQFileAndFormat();

        if (ui_.TotalFQButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedSQ//Total", module_->uniqueName()),
                                                       "Calculated", "Calculated");

            // Add on reference F(Q) data if present
            if (referenceFileAndFormat.hasValidFileAndFormat())
                graph_
                    ->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceData", module_->uniqueName()),
                                                         "Reference F(Q)", "Reference")
                    ->setColour(StockColours::RedStockColour);
        }
        else if (ui_.PartialSQButton->isChecked())
        {
            targetPartials_ = processingData_.valueIf<PartialSet>("WeightedSQ", module_->uniqueName());
            createPartialSetRenderables("WeightedSQ");
        }
        else if (ui_.TotalGRButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedGR//Total", module_->uniqueName()),
                                                       "Calculated", "Calculated");
            auto repGR = graph_->createRenderable<RenderableData1D>(
                fmt::format("{}//RepresentativeTotalGR", module_->uniqueName()), "Via FT", "Calculated");
            repGR->lineStyle().setStipple(LineStipple::HalfDashStipple);
            repGR->setColour(StockColours::GreenStockColour);

            // Add on reference G(r) (from FT of F(Q)) if present
            if (referenceFileAndFormat.hasValidFileAndFormat())
                graph_
                    ->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceDataFT", module_->uniqueName()),
                                                         "Reference G(r) (via FT)", "Reference")
                    ->setColour(StockColours::RedStockColour);
        }
        else if (ui_.PartialGRButton->isChecked())
        {
            targetPartials_ = processingData_.valueIf<PartialSet>("WeightedGR", module_->uniqueName());
            createPartialSetRenderables("WeightedGR");
        }
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

void NeutronSQModuleWidget::on_TotalFQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "F(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void NeutronSQModuleWidget::on_PartialSQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "S(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void NeutronSQModuleWidget::on_TotalGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "G(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void NeutronSQModuleWidget::on_PartialGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "g(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/isotopeData.h"
#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/xRaySQ/gui/xRaySQWidget.h"
#include "modules/xRaySQ/xRaySQ.h"
#include "templates/algorithms.h"

XRaySQModuleWidget::XRaySQModuleWidget(QWidget *parent, XRaySQModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up graph (defaulting to total F(Q))
    graph_ = ui_.PlotWidget->dataViewer();
    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setTitle(1, "F(Q)");
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
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
void XRaySQModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    // Get the filter text (if there is any)
    std::optional<std::string> filterText;
    if (!ui_.FilterEdit->text().isEmpty())
        filterText = ui_.FilterEdit->text().toStdString();

    PairIterator pairs(ps.atomTypeMix().nItems());
    for (auto [first, second] : pairs)
    {
        auto &at1 = ps.atomTypeMix()[first];
        auto &at2 = ps.atomTypeMix()[second];
        const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

        // Filtering - does this 'id' match our filter?
        if (filterText && id.find(filterText.value()) == std::string::npos)
            continue;

        // Full partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->name(), targetPrefix, id),
                                                   fmt::format("{} (Full)", id), "Full");

        // Bound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->name(), targetPrefix, id),
                                                   fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Unbound", module_->name(), targetPrefix, id),
                                                   fmt::format("{} (Unbound)", id), "Unbound");
    }
}

// Update controls within widget
void XRaySQModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) ||
        (!ui_.TotalFQButton->isChecked() && !ui_.TotalGRButton->isChecked() && !targetPartials_))
    {
        ui_.PlotWidget->clearRenderableData();

        // Grab reference data file and format
        const Data1DImportFileFormat &referenceFileAndFormat = module_->referenceFQFileAndFormat();

        if (ui_.TotalFQButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedSQ//Total", module_->name()), "Calculated",
                                                       "Calculated");
            auto boundTotal = graph_->createRenderable<RenderableData1D>(
                fmt::format("{}//WeightedSQ//BoundTotal", module_->name()), "Bound F(Q)", "Calculated");
            boundTotal->setColour(StockColours::GreenStockColour);
            boundTotal->lineStyle().setStipple(LineStipple::DotStipple);
            auto unboundTotal = graph_->createRenderable<RenderableData1D>(
                fmt::format("{}//WeightedSQ//UnboundTotal", module_->name()), "Unbound F(Q)", "Calculated");
            unboundTotal->setColour(StockColours::GreenStockColour);
            unboundTotal->lineStyle().setStipple(LineStipple::HalfDashStipple);

            // Add on reference F(Q) data if present
            if (referenceFileAndFormat.hasFilename())
                graph_
                    ->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceData", module_->name()), "Reference F(Q)",
                                                         "Reference")
                    ->setColour(StockColours::RedStockColour);
        }
        else if (ui_.PartialSQButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("WeightedSQ", module_->name());
            createPartialSetRenderables("WeightedSQ");
        }
        else if (ui_.TotalGRButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedGR//Total", module_->name()), "Calculated",
                                                       "Calculated");
            auto repGR = graph_->createRenderable<RenderableData1D>(fmt::format("{}//RepresentativeTotalGR", module_->name()),
                                                                    "Via FT", "Calculated");
            repGR->lineStyle().setStipple(LineStipple::HalfDashStipple);
            repGR->setColour(StockColours::GreenStockColour);

            // Add on reference G(r) (from FT of F(Q)) if present
            if (referenceFileAndFormat.hasFilename())
                graph_
                    ->createRenderable<RenderableData1D>(fmt::format("{}//ReferenceDataFT", module_->name()),
                                                         "Reference G(r) (via FT)", "Reference")
                    ->setColour(StockColours::RedStockColour);
        }
        else if (ui_.PartialGRButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("WeightedGR", module_->name());
            createPartialSetRenderables("WeightedGR");
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

void XRaySQModuleWidget::on_TotalFQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "F(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void XRaySQModuleWidget::on_PartialSQButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setTitle(1, "S(Q)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void XRaySQModuleWidget::on_TotalGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "G(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void XRaySQModuleWidget::on_PartialGRButton_clicked(bool checked)
{
    if (!checked)
        return;

    graph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    graph_->view().axes().setTitle(1, "g(r)");
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void XRaySQModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void XRaySQModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->setText("");
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

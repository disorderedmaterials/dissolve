// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/isotopeData.h"
#include "gui/dataViewer.h"
#include "gui/helpers/comboBoxController.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/tr/gui/trWidget.h"
#include "modules/tr/tr.h"
#include "templates/algorithms.h"

TRModuleWidget::TRModuleWidget(QWidget *parent, TRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up TR graph
    trGraph_ = ui_.TRPlotWidget->dataViewer();
    // -- Set view
    trGraph_->view().setViewType(View::FlatXYView);
    trGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    trGraph_->view().axes().setMax(0, 10.0);
    trGraph_->view().axes().setTitle(1, "T(r)");
    trGraph_->view().axes().setMin(1, -1.0);
    trGraph_->view().axes().setMax(1, 1.0);
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    trGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    trGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    trGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    trGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    trGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    trGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    trGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    trGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    trGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);
    trGraph_->groupManager().setGroupColouring("Total", RenderableGroup::AutomaticIndividualColouring);
    trGraph_->groupManager().setGroupVerticalShifting("Total", RenderableGroup::IndividualVerticalShifting);

    refreshing_ = false;
}

/*
 * UI
 */

// Create renderables for current target PartialSet
void TRModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    // Get the filter text (if there is any)
    std::optional<std::string> filterText;
    if (!ui_.FilterEdit->text().isEmpty())
        filterText = ui_.FilterEdit->text().toStdString();

    // Set up array matrices for partials
    dissolve::for_each_pair(
        ParallelPolicies::seq, ps.atomTypeMix().begin(), ps.atomTypeMix().end(),
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            const std::string id = std::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

            // Filtering - does this 'id' match our filter?
            if (filterText && id.find(filterText.value()) == std::string::npos)
                return;

            // Full partial
            trGraph_->createRenderable<RenderableData1D>(std::format("{}//{}//{}//Full", module_->name(), targetPrefix, id),
                                                         std::format("{} (Full)", id), "Full");
            // Bound partial
            trGraph_->createRenderable<RenderableData1D>(std::format("{}//{}//{}//Bound", module_->name(), targetPrefix, id),
                                                         std::format("{} (Bound)", id), "Bound");

            // Unbound partial
            trGraph_->createRenderable<RenderableData1D>(std::format("{}//{}//{}//Unbound", module_->name(), targetPrefix, id),
                                                         std::format("{} (Unbound)", id), "Unbound");
        },
        false);
}

// Update controls within widget
void TRModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || (!ui_.TotalButton->isChecked() && !targetPartials_))
    {
        ui_.TRPlotWidget->clearRenderableData();

        if (ui_.TotalButton->isChecked())
        {
            trGraph_->createRenderable<RenderableData1D>(std::format("{}//WeightedTR//Total", module_->name()), "Total T(R)",
                                                         "Calculated");
            auto boundTotal = trGraph_->createRenderable<RenderableData1D>(
                std::format("{}//WeightedTR//BoundTotal", module_->name()), "Bound T(R)", "Calculated");
            boundTotal->lineStyle().setStipple(LineStipple::DotStipple);
            auto unboundTotal = trGraph_->createRenderable<RenderableData1D>(
                std::format("{}//WeightedTR//UnboundTotal", module_->name()), "Unbound T(R)", "Calculated");
            unboundTotal->lineStyle().setStipple(LineStipple::HalfDashStipple);
            auto refTR = trGraph_->createRenderable<RenderableData1D>(std::format("{}//ReferenceTR", module_->name()),
                                                                      "Reference T(r)", "Reference");
            refTR->setColour(StockColours::RedStockColour);
            auto repTR = trGraph_->createRenderable<RenderableData1D>(
                std::format("{}//RepresentativeTR//Total", module_->name()), "Via FT T(R)", "Calculated");
            repTR->lineStyle().setStipple(LineStipple::HalfDashStipple);
            repTR->setColour(StockColours::GreenStockColour);
            auto repBoundTotal = trGraph_->createRenderable<RenderableData1D>(
                std::format("{}//RepresentativeTR//BoundTotal", module_->name()), "Via FT Bound T(R)", "Calculated");
            repBoundTotal->setColour(StockColours::GreenStockColour);
            repBoundTotal->lineStyle().setStipple(LineStipple::DotStipple);
            auto repUnboundTotal = trGraph_->createRenderable<RenderableData1D>(
                std::format("{}//RepresentativeTR//UnboundTotal", module_->name()), "Via FT Unbound T(R)", "Calculated");
            repUnboundTotal->setColour(StockColours::GreenStockColour);
            repUnboundTotal->lineStyle().setStipple(LineStipple::HalfDashStipple);
        }
        else if (ui_.PartialsButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("WeightedTR", module_->name());
            createPartialSetRenderables("WeightedTR");
        }
        else if (ui_.RepresentativePartialsButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("RepresentativeTR", module_->name());
            createPartialSetRenderables("RepresentativeTR");
        }
    }

    // Validate renderables if they need it
    trGraph_->validateRenderables(dissolve_.processingModuleData());

    trGraph_->postRedisplay();
    ui_.TRPlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void TRModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    trGraph_->view().axes().setTitle(1, "t(r)");
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_TotalButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(false);

    trGraph_->view().axes().setTitle(1, "T(r)");
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_RepresentativePartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    trGraph_->view().axes().setTitle(1, "t(r)");
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void TRModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->setText("");
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/isotopeData.h"
#include "gui/dataViewer.hui"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/sq/gui/sqWidget.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

SQModuleWidget::SQModuleWidget(QWidget *parent, SQModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
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

/*
 * UI
 */

// Create renderables for current target PartialSet
void SQModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
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
        sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->name(), targetPrefix, id),
                                                     fmt::format("{} (Full)", id), "Full");

        // Bound partial
        sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->name(), targetPrefix, id),
                                                     fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Unbound", module_->name(), targetPrefix, id),
                                                     fmt::format("{} (Unbound)", id), "Unbound");
    }
}

// Update controls within widget
void SQModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || (!ui_.TotalButton->isChecked() && !targetPartials_))
    {
        ui_.SQPlotWidget->clearRenderableData();

        if (ui_.PartialsButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("UnweightedSQ", module_->name());
            createPartialSetRenderables("UnweightedSQ");
        }
        else
            sqGraph_->createRenderable<RenderableData1D>(fmt::format("{}//UnweightedSQ//Total", module_->name()), "Total",
                                                         "Calc");
    }

    // Validate renderables if they need it
    sqGraph_->validateRenderables(dissolve_.processingModuleData());

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

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void SQModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    sqGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    sqGraph_->view().axes().setTitle(1, "S(Q)");

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void SQModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void SQModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->setText("");
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

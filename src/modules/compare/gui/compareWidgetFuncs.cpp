// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/compare/compare.h"
#include "modules/compare/gui/compareWidget.h"

CompareModuleWidget::CompareModuleWidget(QWidget *parent, CompareModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Check data1D button by default
    ui_.Data1DButton->setChecked(true);

    // Set up the main graph
    graph_ = ui_.PlotWidget->dataViewer();

    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    graph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    // -- Set group styling
    graph_->groupManager().setGroupFixedColour("Delta", StockColours::BlackStockColour);
    graph_->groupManager().setGroupColouring("Delta", RenderableGroup::FixedGroupColouring);
    graph_->groupManager().setGroupStipple("Delta", LineStipple::QuarterDashStipple);
    graph_->groupManager().setGroupVerticalShifting("Delta", RenderableGroup::IndividualVerticalShifting);

    graph_->groupManager().setGroupFixedColour("DataA", StockColours::RedStockColour);
    graph_->groupManager().setGroupColouring("DataA", RenderableGroup::FixedGroupColouring);
    graph_->groupManager().setGroupVerticalShifting("DataA", RenderableGroup::IndividualVerticalShifting);

    graph_->groupManager().setGroupFixedColour("DataB", StockColours::LightGreenStockColour);
    graph_->groupManager().setGroupColouring("DataB", RenderableGroup::FixedGroupColouring);
    graph_->groupManager().setGroupVerticalShifting("DataB", RenderableGroup::IndividualVerticalShifting);

    refreshing_ = false;
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

/*
 * UI
 */

// Update controls within widget
void CompareModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        refreshing_ = true;

        ui_.PlotWidget->clearRenderableData();

        // Create the renderables for Data1D
        if (ui_.Data1DButton->isChecked())
        {
            auto dSourceCount = 1;
            for (auto &[dataSourceA, dataSourceB] : module_->data1dSources())
            {
                // Render the difference (delta) between the datasets
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_Delta", module_->name(), dSourceCount),
                                                           "Delta", "Delta");
                // Render DataA in the pair
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_DataA", module_->name(), dSourceCount),
                                                           fmt::format("{}", dataSourceA->dataName()), "DataA");
                // Render DataB in the pair
                graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_DataB", module_->name(), dSourceCount),
                                                           fmt::format("{}", dataSourceB->dataName()), "DataB");

                // Validate renderables if they need it
                graph_->validateRenderables(dissolve_.processingModuleData());

                graph_->view().axes().setMax(1, std::max(dataSourceA->data().maxValue(), dataSourceB->data().maxValue()) + 5.0);

                ++dSourceCount;
            }
        }

        ui_.PlotWidget->updateToolbar();

        graph_->postRedisplay();

        refreshing_ = false;
    }
}

void CompareModuleWidget::on_Data1DButton_clicked(bool checked)
{
    if (checked)
    {
        updateControls(ModuleWidget::RecreateRenderablesFlag);
    }
}
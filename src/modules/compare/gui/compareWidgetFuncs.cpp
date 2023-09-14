// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/compare/compare.h"
#include "modules/compare/gui/compareWidget.h"
#include <QDebug>

CompareModuleWidget::CompareModuleWidget(QWidget *parent, CompareModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up the main graph
    graph_ = ui_.PlotWidget->dataViewer();

    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    graph_->groupManager().setGroupFixedColour("Delta", StockColours::BlackStockColour);
    graph_->groupManager().setGroupColouring("Delta", RenderableGroup::FixedGroupColouring);
    graph_->groupManager().setGroupStipple("Delta", LineStipple::QuarterDashStipple);

    graph_->groupManager().setGroupFixedColour("DataA", StockColours::RedStockColour);
    graph_->groupManager().setGroupColouring("DataA", RenderableGroup::FixedGroupColouring);

    graph_->groupManager().setGroupFixedColour("DataB", StockColours::LightGreenStockColour);
    graph_->groupManager().setGroupColouring("DataB", RenderableGroup::FixedGroupColouring);

    refreshing_ = false;
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

/*
 * UI
 */

// Update controls within widget
void CompareModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{

    // If data sources have been added, create buttons
    if (module_->dataSources().size() > buttons_.size())
    {
        for (auto it = module_->dataSources().begin() + buttons_.size(); it != module_->dataSources().end(); ++it)
        {
            auto dataButton = new QPushButton(this);
            dataButton->setCheckable(true);
            dataButton->setAutoExclusive(true);
            dataButton->setText(QString::fromStdString(fmt::format("Compare {}", it - module_->dataSources().begin() + 1)));
            connect(dataButton, SIGNAL(clicked(bool)), this, SLOT(onButtonChecked(bool)));
            ui_.ButtonLayout->addWidget(dataButton);
            buttons_.push_back(dataButton);
        }
    }

    // If data sources have been removed, remove buttons
    else if (module_->dataSources().size() < buttons_.size())
    {
        // Iterate over extra elements in button vector
        for (auto it = buttons_.rbegin(); it != buttons_.rbegin() + module_->dataSources().size() - 1; ++it)
        {
            ui_.ButtonLayout->removeWidget(*it);
            buttons_.pop_back();
        }
    }

    // Return if no data sources are specified
    if (buttons_.size() == 0)
    {
        return;
    }

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        refreshing_ = true;

        ui_.PlotWidget->clearRenderableData();

        // Get the index of the checked button
        auto index =
            std::find_if(buttons_.begin(), buttons_.end(), [&](auto button) { return button->isChecked(); }) - buttons_.begin();

        // If no buttons are checked, check the first one by default
        if (index == buttons_.size() && buttons_.back()->isChecked() == false)
        {
            buttons_.front()->setChecked(true);
            index = 0;
        }

        // Get the data pair at the same index as the checked button
        auto &[dataSourceA, dataSourceB] = module_->dataSources()[index];

        // Render the difference (delta) between the datasets
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_Delta", module_->name(), index + 1), "Delta",
                                                   "Delta");
        // Render DataA in the pair
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_DataA", module_->name(), index + 1),
                                                   fmt::format("{}", dataSourceA.dataName()), "DataA");
        // Render DataB in the pair
        graph_->createRenderable<RenderableData1D>(fmt::format("{}//Pair{}_DataB", module_->name(), index + 1),
                                                   fmt::format("{}", dataSourceB.dataName()), "DataB");

        // Set the button tooltip
        buttons_[index]->setToolTip(
            QString::fromStdString(fmt::format("{} vs {}", dataSourceA.dataName(), dataSourceB.dataName())));

        // Validate renderables if they need it
        graph_->validateRenderables(dissolve_.processingModuleData());

        ui_.PlotWidget->updateToolbar();

        graph_->postRedisplay();

        refreshing_ = false;
    }
}

void CompareModuleWidget::onButtonChecked(bool checked)
{
    if (!checked)
    {
        return;
    }
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}
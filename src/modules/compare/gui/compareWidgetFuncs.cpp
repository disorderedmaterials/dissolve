// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/compare/compare.h"
#include "modules/compare/gui/compareWidget.h"

CompareModuleWidget::CompareModuleWidget(QWidget *parent, CompareModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up the main graph
    graph_ = ui_.PlotWidget->dataViewer();
    rangeErrorTable_ = ui_.RangeErrorTable;

    auto index = 0;
    for (auto &[dataA, dataB] : module->dataSources())
    {
        auto dataButton = new QPushButton(this);
        dataButton->setText(QString::fromStdString(fmt::format("Compare{}", index)));
        ui_.ButtonLayout->addWidget(dataButton);
        buttons_.push_back(dataButton);

        ++index;
    }
}

/*
 * UI
 */

// Update controls within widget
void CompareModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{

    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || ui_.PlotWidget->dataViewer()->renderables().empty())
    {
        // Get the index of the checked button
        auto index =
            std::find_if(buttons_.begin(), buttons_.end(), [&](auto button) { return button->isChecked(); }) - buttons_.begin();
        // Get the data pair at the same index as the checked button
        auto &[dataSourceA, dataSourceB] = module_->dataSources()[index];
        // Render DataA in the pair
        graph_->createRenderable<RenderableData1D>(fmt::format("Pair{}_DataA//Compare", index),
                                                   fmt::format("{}", dataSourceA.dataName()));
        // Render DataB in the pair
        graph_->createRenderable<RenderableData1D>(fmt::format("Pair{}_DataA//Compare", index),
                                                   fmt::format("{}", dataSourceA.dataName()));
        // Render the difference (delta) between the datasets
        graph_->createRenderable<RenderableData1D>(fmt::format("Pair{}_Delta//Compare", index), "Delta");
    }
}
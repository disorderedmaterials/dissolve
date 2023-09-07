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

    for (auto &[dataA, dataB] : module->dataSources())
    {
        auto dataButton = new QPushButton(this);
        dataButton->setText(QString::fromStdString(fmt::format("{} vs {}", dataA.get()->tag(), dataB.get()->tag())));
        ui_.ButtonLayout->addWidget(dataButton);
        buttons_.push_back(&dataButton);
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
    }
}
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/modulewidget.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

BraggModuleWidget::BraggModuleWidget(QWidget *parent, const GenericList &processingData, BraggModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up Bragg reflections graph
    reflectionsGraph_ = ui_.ReflectionsPlotWidget->dataViewer();
    // -- Set view
    reflectionsGraph_->view().setViewType(View::FlatXYView);
    reflectionsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    reflectionsGraph_->view().axes().setMax(0, 10.0);
    reflectionsGraph_->view().axes().setTitle(1, "Intensity");
    reflectionsGraph_->view().axes().setMin(1, -1.0);
    reflectionsGraph_->view().axes().setMax(1, 1.0);
    reflectionsGraph_->view().setAutoFollowType(View::AllAutoFollow);

    // Set up total G(r) graph
    totalsGraph_ = ui_.TotalsPlotWidget->dataViewer();
    // -- Set view
    totalsGraph_->view().setViewType(View::FlatXYView);
    totalsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    totalsGraph_->view().axes().setMax(0, 10.0);
    totalsGraph_->view().axes().setTitle(1, "Intensity");
    totalsGraph_->view().axes().setMin(1, -1.0);
    totalsGraph_->view().axes().setMax(1, 1.0);
    totalsGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    totalsGraph_->groupManager().setGroupColouring("Totals", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;

    currentConfiguration_ = nullptr;

    updateControls(ModuleWidget::UpdateType::Normal);

    setGraphDataTargets();
}

BraggModuleWidget::~BraggModuleWidget() {}

// Update controls within widget
void BraggModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    ui_.ReflectionsPlotWidget->updateToolbar();
    ui_.TotalsPlotWidget->updateToolbar();

    reflectionsGraph_->postRedisplay();
    totalsGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void BraggModuleWidget::setGraphDataTargets()
{
    if (!module_)
        return;

    // Add Configuration targets to the combo box
    ui_.TargetCombo->clear();
    for (const auto *cfg : module_->targetConfigurations())
        ui_.TargetCombo->addItem(QString::fromStdString(std::string(cfg->name())), VariantPointer<Configuration>(cfg));

    // Loop over Configurations and add total Bragg F(Q)
    for (const auto *cfg : module_->targetConfigurations())
    {
        // Original F(Q)
        totalsGraph_->createRenderable<RenderableData1D>(fmt::format("{}//OriginalBragg//Total", cfg->niceName()),
                                                         cfg->niceName(), "Totals");
    }
}

void BraggModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
    // Remove any current data
    reflectionsGraph_->clearRenderables();

    // Get target Configuration
    currentConfiguration_ = (Configuration *)VariantPointer<Configuration>(ui_.TargetCombo->itemData(index));
    if (!currentConfiguration_)
        return;

    auto &types = currentConfiguration_->usedAtomTypesList();
    for_each_pair(types.begin(), types.end(), [&](int n, const AtomTypeData &atd1, int m, const AtomTypeData &atd2) {
        const std::string id = fmt::format("{}-{}", atd1.atomTypeName(), atd2.atomTypeName());

        // Original S(Q)
        reflectionsGraph_->createRenderable<RenderableData1D>(
            fmt::format("{}//OriginalBragg//{}", currentConfiguration_->niceName(), id), fmt::format("Full//{}", id), "Full");
    });

    reflectionsGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
}

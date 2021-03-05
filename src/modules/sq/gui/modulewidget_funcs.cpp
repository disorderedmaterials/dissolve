// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/sq/gui/modulewidget.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

SQModuleWidget::SQModuleWidget(QWidget *parent, SQModule *module, Dissolve &dissolve)
    : ModuleWidget(parent), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up partial S(Q) graph

    partialsGraph_ = ui_.PartialSQPlotWidget->dataViewer();

    partialsGraph_->view().setViewType(View::FlatXYView);
    partialsGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    partialsGraph_->view().axes().setMax(0, 10.0);
    partialsGraph_->view().axes().setTitle(1, "S(Q)");
    partialsGraph_->view().axes().setMin(1, -1.0);
    partialsGraph_->view().axes().setMax(1, 1.0);
    partialsGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    partialsGraph_->view().setAutoFollowType(View::AllAutoFollow);
    partialsGraph_->groupManager().setGroupColouring("Partial S(Q)", RenderableGroup::AutomaticIndividualColouring);
    partialsGraph_->groupManager().setGroupVerticalShifting("Partial S(Q)", RenderableGroup::IndividualVerticalShifting);

    // Set up total F(Q) graph

    totalGraph_ = ui_.TotalFQPlotWidget->dataViewer();

    totalGraph_->view().setViewType(View::FlatXYView);
    totalGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    totalGraph_->view().axes().setMax(0, 10.0);
    totalGraph_->view().axes().setTitle(1, "S(Q)");
    totalGraph_->view().axes().setMin(1, -1.0);
    totalGraph_->view().axes().setMax(1, 1.0);
    totalGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    totalGraph_->view().setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

SQModuleWidget::~SQModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void SQModuleWidget::updateControls(int flags)
{
    ui_.PartialSQPlotWidget->updateToolbar();
    ui_.TotalFQPlotWidget->updateToolbar();

    partialsGraph_->postRedisplay();
    totalGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void SQModuleWidget::setGraphDataTargets(SQModule *module)
{
    // Partial S(Q)
    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        partialsGraph_->createRenderable(Renderable::Data1DRenderable,
                                         fmt::format("{}//UnweightedSQ//{}//Full", module_->uniqueName(), id), id,
                                         "Partial S(Q)");
    });

    // Add calculated total F(Q)
    totalGraph_->createRenderable(Renderable::Data1DRenderable, fmt::format("{}//UnweightedSQ//Total", module_->uniqueName()),
                                  "Calculated", "Total F(Q)");
}

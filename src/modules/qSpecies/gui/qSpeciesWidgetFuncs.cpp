// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "math/integrator.h"
#include "modules/qSpecies/gui/qSpeciesWidget.h"
#include "modules/qSpecies/qSpecies.h"

QSpeciesModuleWidget::QSpeciesModuleWidget(QWidget *parent, QSpeciesModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    qSpeciesGraph_ = ui_.QSpeciesPlotWidget->dataViewer();

    auto &view = qSpeciesGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Qn");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Normalised Frequency");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void QSpeciesModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        qSpeciesGraph_->clearRenderables();

    if (qSpeciesGraph_->renderables().empty())
        qSpeciesGraph_->createRenderable<RenderableData1D>(fmt::format("{}//QSpecies", module_->name()), "Q-Species");

    // Update Oxygen Sites Labels
    auto oSitesHistogram = dissolve_.processingModuleData().valueOr("OSitesHistogram", module_->name(), IntegerHistogram1D());
    auto averages = oSitesHistogram.averages();
    auto sum = Integrator::absSum(oSitesHistogram.data());

    ui_.FOResultFrame->setText(averages[0] / sum);
    ui_.NBOResultFrame->setText(averages[1] / sum);
    ui_.BOResultFrame->setText(averages[2] / sum);
    ui_.OddOResultsFrame->setText((sum - averages[0] - averages[1] - averages[2]) / sum);

    // Validate renderables if they need it
    qSpeciesGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.QSpeciesPlotWidget->updateToolbar();

    qSpeciesGraph_->postRedisplay();
}

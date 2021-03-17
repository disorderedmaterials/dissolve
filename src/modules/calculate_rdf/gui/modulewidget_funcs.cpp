// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderabledata1d.h"
#include "modules/calculate_rdf/gui/modulewidget.h"
#include "modules/calculate_rdf/rdf.h"

CalculateRDFModuleWidget::CalculateRDFModuleWidget(QWidget *parent, CalculateRDFModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget->dataViewer();

    auto &view = rdfGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "g(r)");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    setGraphDataTargets();

    updateControls();

    refreshing_ = false;
}

// Update controls within widget
void CalculateRDFModuleWidget::updateControls(int flags)
{
    ui_.RDFPlotWidget->updateToolbar();

    rdfGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateRDFModuleWidget::setGraphDataTargets()
{
    // Remove any current data
    rdfGraph_->clearRenderables();

    if (!module_)
        return;

    // Loop over Configuration targets in Module
    for (const auto *cfg : module_->targetConfigurations())
    {
        // Calculated RDF
        auto rdf = rdfGraph_->createRenderable(Renderable::Data1DRenderable,
                                               fmt::format("{}//Process1D//{}//RDF", module_->uniqueName(), cfg->niceName()),
                                               fmt::format("RDF//{}", cfg->niceName()), cfg->niceName());
        rdf->setColour(StockColours::BlueStockColour);
    }
}

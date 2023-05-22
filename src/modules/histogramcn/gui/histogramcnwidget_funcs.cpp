// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderabledata1d.h"
#include "main/dissolve.h"
#include "modules/histogramcn/gui/histogramcnwidget.h"
#include "modules/histogramcn/histogramcn.h"

HistogramCNModuleWidget::HistogramCNModuleWidget(QWidget *parent, HistogramCNModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    cnGraph_ = ui_.CNPlotWidget->dataViewer();

    auto &view = cnGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Coordination Number");
    view.axes().setMax(0, 10.0);
    view.axes().setTitle(1, "Population");
    view.axes().setMin(1, 0.0);
    view.axes().setMax(1, 1.0);
    view.setAutoFollowType(View::AllAutoFollow);

    refreshing_ = false;
}

// Update controls within widget
void HistogramCNModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags) {}

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
}

// Update controls within widget
void HistogramCNModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags) {}

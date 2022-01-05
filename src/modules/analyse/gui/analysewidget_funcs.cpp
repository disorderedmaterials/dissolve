// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/analysewidget.h"

AnalyseModuleWidget::AnalyseModuleWidget(QWidget *parent, AnalyseModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set Procedure target
    ui_.EditorWidget->setUp(&module_->analyser(), dissolve_.coreData());

    refreshing_ = false;
}

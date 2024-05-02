// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/geomOpt/geomOpt.h"
#include "modules/geomOpt/gui/geomOptWidget.h"

GeometryOptimisationModuleWidget::GeometryOptimisationModuleWidget(QWidget *parent, GeometryOptimisationModule *module,
                                                                   Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

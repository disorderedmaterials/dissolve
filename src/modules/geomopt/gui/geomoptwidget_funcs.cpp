// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/geomoptwidget.h"

GeometryOptimisationModuleWidget::GeometryOptimisationModuleWidget(QWidget *parent, GeometryOptimisationModule *module,
                                                                   Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/modulewidget.h"

GeometryOptimisationModuleWidget::GeometryOptimisationModuleWidget(QWidget *parent, GeometryOptimisationModule *module)
    : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/modulewidget.h"

GeometryOptimisationModuleWidget::GeometryOptimisationModuleWidget(QWidget *parent, const GenericList &processingData,
                                                                   GeometryOptimisationModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/skeleton/gui/skeletonwidget.h"
#include "modules/skeleton/skeleton.h"

SkeletonModuleWidget::SkeletonModuleWidget(QWidget *parent, SkeletonModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

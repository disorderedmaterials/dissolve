// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/drivenMD/drivenMD.h"
#include "modules/drivenMD/gui/drivenMDWidget.h"

DrivenMDModuleWidget::DrivenMDModuleWidget(QWidget *parent, DrivenMDModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

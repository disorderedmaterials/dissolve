// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/epsrManager/epsrManager.h"
#include "modules/epsrManager/gui/epsrManagerWidget.h"

EPSRManagerModuleWidget::EPSRManagerModuleWidget(QWidget *parent, EPSRManagerModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

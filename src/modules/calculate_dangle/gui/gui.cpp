// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_dangle/dangle.h"
#include "modules/calculate_dangle/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateDAngleModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateDAngleModuleWidget(parent, dissolve.processingModuleData(), this);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_axisangle/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateAxisAngleModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateAxisAngleModuleWidget(parent, dissolve.processingModuleData(), this);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_angle/angle.h"
#include "modules/calculate_angle/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateAngleModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateAngleModuleWidget(parent, dissolve.processingModuleData(), this);
}

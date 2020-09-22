// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/calculate_angle/angle.h"
#include "modules/calculate_angle/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateAngleModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateAngleModuleWidget(parent, this);
}

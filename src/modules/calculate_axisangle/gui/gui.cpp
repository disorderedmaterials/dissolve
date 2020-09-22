// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_axisangle/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateAxisAngleModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateAxisAngleModuleWidget(parent, this);
}

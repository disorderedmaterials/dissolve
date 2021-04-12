// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *EnergyModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new EnergyModuleWidget(parent, dissolve.processingModuleData(), this);
}

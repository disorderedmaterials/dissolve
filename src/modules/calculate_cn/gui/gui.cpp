// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_cn/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateCNModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateCNModuleWidget(parent, dissolve.processingModuleData(), this);
}

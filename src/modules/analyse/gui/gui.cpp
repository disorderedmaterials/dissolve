// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/analyse/analyse.h"
#include "modules/analyse/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *AnalyseModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new AnalyseModuleWidget(parent, dissolve.processingModuleData(), this, dissolve.coreData());
}

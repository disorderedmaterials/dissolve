// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *DataTestModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new DataTestModuleWidget(parent, dissolve.processingModuleData(), this);
}

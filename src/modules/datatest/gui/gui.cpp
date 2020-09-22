// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *DataTestModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new DataTestModuleWidget(parent, this);
}

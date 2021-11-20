// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/test/gui/testwidget.h"
#include "modules/test/test.h"

// Return a new widget controlling this Module
ModuleWidget *TestModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new TestModuleWidget(parent, this, dissolve);
}

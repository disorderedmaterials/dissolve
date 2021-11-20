// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/braggwidget.h"

// Return a new widget controlling this Module
ModuleWidget *BraggModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new BraggModuleWidget(parent, this, dissolve);
}

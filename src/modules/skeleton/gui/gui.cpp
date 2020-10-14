// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/skeleton/gui/modulewidget.h"
#include "modules/skeleton/skeleton.h"

// Return a new widget controlling this Module
ModuleWidget *SkeletonModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new SkeletonModuleWidget(parent, this);
}

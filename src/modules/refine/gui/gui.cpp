// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/refine/gui/modulewidget.h"
#include "modules/refine/refine.h"

// Return a new widget controlling this Module
ModuleWidget *RefineModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new RefineModuleWidget(parent, this, dissolve);
}

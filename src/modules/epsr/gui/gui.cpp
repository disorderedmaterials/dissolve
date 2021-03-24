// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *EPSRModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new EPSRModuleWidget(parent, dissolve.processingModuleData(), this, dissolve);
}

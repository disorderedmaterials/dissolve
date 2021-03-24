// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/neutronsq/gui/modulewidget.h"
#include "modules/neutronsq/neutronsq.h"

// Return a new widget controlling this Module
ModuleWidget *NeutronSQModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new NeutronSQModuleWidget(parent, dissolve.processingModuleData(), this, dissolve);
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/neutronsq/gui/modulewidget.h"
#include "modules/neutronsq/neutronsq.h"

// Return a new widget controlling this Module
ModuleWidget *NeutronSQModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new NeutronSQModuleWidget(parent, this, dissolve);
}

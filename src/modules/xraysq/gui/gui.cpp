// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/xraysq/gui/modulewidget.h"
#include "modules/xraysq/xraysq.h"

// Return a new widget controlling this Module
ModuleWidget *XRaySQModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new XRaySQModuleWidget(parent, dissolve.processingModuleData(), this, dissolve);
}

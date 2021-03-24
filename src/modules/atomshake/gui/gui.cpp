// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *AtomShakeModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new AtomShakeModuleWidget(parent, dissolve.processingModuleData(), this);
}

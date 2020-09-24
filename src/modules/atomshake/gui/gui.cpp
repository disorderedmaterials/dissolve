// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *AtomShakeModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new AtomShakeModuleWidget(parent, this);
}

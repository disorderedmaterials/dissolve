// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/atomShake/atomShake.h"
#include "modules/atomShake/gui/atomShakeWidget.h"

AtomShakeModuleWidget::AtomShakeModuleWidget(QWidget *parent, AtomShakeModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

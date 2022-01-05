// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/atomshakewidget.h"

AtomShakeModuleWidget::AtomShakeModuleWidget(QWidget *parent, AtomShakeModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

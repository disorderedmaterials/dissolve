// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"
#include "modules/atomshake/gui/modulewidget.h"

AtomShakeModuleWidget::AtomShakeModuleWidget(QWidget *parent, AtomShakeModule *module) : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

AtomShakeModuleWidget::~AtomShakeModuleWidget() {}

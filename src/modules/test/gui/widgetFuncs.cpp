// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/test/gui/widget.h"
#include "modules/test/test.h"

TestModuleWidget::TestModuleWidget(QWidget *parent, TestModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

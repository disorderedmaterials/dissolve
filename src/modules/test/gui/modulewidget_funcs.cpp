// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/test/gui/modulewidget.h"
#include "modules/test/test.h"

TestModuleWidget::TestModuleWidget(QWidget *parent, TestModule *module) : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

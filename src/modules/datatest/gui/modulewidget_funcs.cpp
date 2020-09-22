// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/modulewidget.h"

DataTestModuleWidget::DataTestModuleWidget(QWidget *parent, DataTestModule *module) : ModuleWidget(parent), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

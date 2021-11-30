// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/datatest/datatest.h"
#include "modules/datatest/gui/datatestwidget.h"

DataTestModuleWidget::DataTestModuleWidget(QWidget *parent, DataTestModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

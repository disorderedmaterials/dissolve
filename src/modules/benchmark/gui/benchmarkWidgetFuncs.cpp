// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/benchmarkWidget.h"

BenchmarkModuleWidget::BenchmarkModuleWidget(QWidget *parent, BenchmarkModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/modulewidget.h"

BenchmarkModuleWidget::BenchmarkModuleWidget(QWidget *parent, const GenericList &processingData, BenchmarkModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;
}

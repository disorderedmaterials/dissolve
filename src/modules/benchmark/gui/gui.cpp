// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/benchmark/benchmark.h"
#include "modules/benchmark/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *BenchmarkModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new BenchmarkModuleWidget(parent, dissolve.processingModuleData(), this);
}

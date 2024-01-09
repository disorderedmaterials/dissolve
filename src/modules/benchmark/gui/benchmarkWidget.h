// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/benchmark/gui/ui_benchmarkWidget.h"
#include "modules/widget.h"

// Forward Declarations
class BenchmarkModule;

// Module Widget
class BenchmarkModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    BenchmarkModule *module_;

    public:
    BenchmarkModuleWidget(QWidget *parent, BenchmarkModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::BenchmarkModuleWidget ui_;
};

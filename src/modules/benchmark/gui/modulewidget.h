// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/benchmark/gui/ui_modulewidget.h"

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
    BenchmarkModuleWidget(QWidget *parent, const GenericList &processingData, BenchmarkModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::BenchmarkModuleWidget ui_;
};

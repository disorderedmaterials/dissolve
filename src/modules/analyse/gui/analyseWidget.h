// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/analyse/gui/ui_analyseWidget.h"
#include "modules/widget.h"

// Forward Declarations
class AnalyseModule;
class Dissolve;

// Module Widget
class AnalyseModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AnalyseModuleWidget(QWidget *parent, AnalyseModule *module, Dissolve &dissolve);
    ~AnalyseModuleWidget() override = default;

    private:
    // Associated Module
    AnalyseModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AnalyseModuleWidget ui_;
};

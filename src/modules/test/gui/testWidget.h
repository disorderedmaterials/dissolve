// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/test/gui/ui_testWidget.h"
#include "modules/widget.h"

// Forward Declarations
class TestModule;

// Module Widget
class TestModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    TestModule *module_;

    public:
    TestModuleWidget(QWidget *parent, TestModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::TestModuleWidget ui_;
};

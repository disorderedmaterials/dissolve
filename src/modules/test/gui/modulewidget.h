// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/test/gui/ui_modulewidget.h"

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
    TestModuleWidget(QWidget *parent, const GenericList &processingData, TestModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::TestModuleWidget ui_;
};

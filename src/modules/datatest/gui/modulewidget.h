// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/datatest/gui/ui_modulewidget.h"

// Forward Declarations
class DataTestModule;

// Module Widget
class DataTestModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    DataTestModule *module_;

    public:
    DataTestModuleWidget(QWidget *parent, const GenericList &processingData, DataTestModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DataTestModuleWidget ui_;
};

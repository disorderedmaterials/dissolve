// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "modules/datatest/gui/ui_datatestwidget.h"
#include "modules/widget.h"

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
    DataTestModuleWidget(QWidget *parent, DataTestModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DataTestModuleWidget ui_;
};

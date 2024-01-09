// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/avgMol/gui/ui_avgMolWidget.h"
#include "modules/widget.h"

// Forward Declarations
class AvgMolModule;

// Module Widget
class AvgMolModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    AvgMolModule *module_;

    public:
    AvgMolModuleWidget(QWidget *parent, AvgMolModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AvgMolModuleWidget ui_;
};

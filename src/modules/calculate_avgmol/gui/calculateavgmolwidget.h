// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "modules/calculate_avgmol/gui/ui_calculateavgmolwidget.h"
#include "modules/widget.h"

// Forward Declarations
class CalculateAvgMolModule;

// Module Widget
class CalculateAvgMolModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    CalculateAvgMolModule *module_;

    public:
    CalculateAvgMolModuleWidget(QWidget *parent, CalculateAvgMolModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateAvgMolModuleWidget ui_;
};

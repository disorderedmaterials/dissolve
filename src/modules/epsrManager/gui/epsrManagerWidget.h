// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/epsrManager/gui/ui_epsrManagerWidget.h"
#include "modules/widget.h"

// Forward Declarations
class EPSRManagerModule;
class Dissolve;

// Module Widget
class EPSRManagerModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EPSRManagerModuleWidget(QWidget *parent, EPSRManagerModule *module, Dissolve &dissolve);
    ~EPSRManagerModuleWidget() override = default;

    private:
    // Associated Module
    EPSRManagerModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EPSRManagerModuleWidget ui_;
};

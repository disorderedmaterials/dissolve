// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/drivenMD/gui/ui_drivenMDWidget.h"
#include "modules/widget.h"

// Forward Declarations
class DrivenMDModule;
class Dissolve;
class Module;

// Module Widget
class DrivenMDModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    DrivenMDModuleWidget(QWidget *parent, DrivenMDModule *module, Dissolve &dissolve);
    ~DrivenMDModuleWidget() override = default;

    private:
    // Associated Module
    DrivenMDModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DrivenMDModuleWidget ui_;
};

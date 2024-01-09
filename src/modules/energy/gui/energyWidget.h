// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/energy/gui/ui_energyWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class PartialSet;
class EnergyModule;
class DataViewer;

// Module Widget
class EnergyModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EnergyModuleWidget(QWidget *parent, EnergyModule *module, Dissolve &dissolve);
    ~EnergyModuleWidget() override = default;

    private:
    // Associated Module
    EnergyModule *module_;
    // DataViewer contained within this widget
    DataViewer *energyGraph_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EnergyModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

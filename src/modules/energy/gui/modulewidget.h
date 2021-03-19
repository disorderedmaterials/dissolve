// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/energy/gui/ui_modulewidget.h"

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
    EnergyModuleWidget(QWidget *parent, const GenericList &processingData, EnergyModule *module);
    ~EnergyModuleWidget();

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
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private:
    // Current Configuration whose data is being displayed
    Configuration *currentConfiguration_;

    private:
    // Set data targets in graphs
    void setGraphDataTargets(EnergyModule *module);

    private slots:
    void on_TargetCombo_currentIndexChanged(int index);
};

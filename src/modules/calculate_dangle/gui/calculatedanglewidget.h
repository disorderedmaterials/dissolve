// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "modules/calculate_dangle/gui/ui_calculatedanglewidget.h"
#include "modules/widget.h"

// Forward Declarations
class CalculateDAngleModule;
class DataViewer;

// Module Widget
class CalculateDAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    CalculateDAngleModule *module_;

    public:
    CalculateDAngleModuleWidget(QWidget *parent, CalculateDAngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateDAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfGraph_, *angleGraph_, *dAngleGraph_;

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_angle/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateAngleModule;
class DataViewer;

// Module Widget
class CalculateAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    CalculateAngleModule *module_;

    public:
    CalculateAngleModuleWidget(QWidget *parent, const GenericList &processingData, CalculateAngleModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfABGraph_, *rdfBCGraph_, *angleGraph_, *dAngleABGraph_, *dAngleBCGraph_;

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(CalculateAngleModule *module);

    private slots:
};

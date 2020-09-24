// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    CalculateAngleModuleWidget(QWidget *parent, CalculateAngleModule *module);

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
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * State I/O
     */
    public:
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
    // Read widget state through specified LineParser
    bool readState(LineParser &parser);

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(CalculateAngleModule *module);

    private slots:
};

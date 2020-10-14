// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_axisangle/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateAxisAngleModule;
class DataViewer;

// Module Widget
class CalculateAxisAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    CalculateAxisAngleModule *module_;

    public:
    CalculateAxisAngleModuleWidget(QWidget *parent, CalculateAxisAngleModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateAxisAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfGraph_, *angleGraph_, *dAngleGraph_;

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
    void setGraphDataTargets(CalculateAxisAngleModule *module);

    private slots:
};

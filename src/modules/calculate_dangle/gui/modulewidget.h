// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_dangle/gui/ui_modulewidget.h"

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
    CalculateDAngleModuleWidget(QWidget *parent, CalculateDAngleModule *module);

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
    void setGraphDataTargets(CalculateDAngleModule *module);

    private slots:
};

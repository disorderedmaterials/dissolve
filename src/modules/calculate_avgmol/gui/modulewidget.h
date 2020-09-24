// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_avgmol/gui/ui_modulewidget.h"

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
    CalculateAvgMolModuleWidget(QWidget *parent, CalculateAvgMolModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateAvgMolModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);
    // Disable sensitive controls within widget
    void disableSensitiveControls();
    // Enable sensitive controls within widget
    void enableSensitiveControls();

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
    private slots:
};

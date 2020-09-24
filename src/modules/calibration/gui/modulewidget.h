// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calibration/gui/ui_modulewidget.h"

// Forward Declarations
class CalibrationModule;
class Dissolve;
class Module;
class PartialSet;
class DataViewer;

// Module Widget
class CalibrationModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    CalibrationModuleWidget(QWidget *parent, CalibrationModule *module);
    ~CalibrationModuleWidget();

    private:
    // Associated Module
    CalibrationModule *module_;
    // DataViewer contained within this widget
    DataViewer *dataView_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalibrationModuleWidget ui_;

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
};

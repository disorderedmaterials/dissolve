// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/axisAngle/gui/ui_axisAngleWidget.h"
#include "modules/widget.h"

// Forward Declarations
class AxisAngleModule;
class DataViewer;

// Module Widget
class AxisAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    AxisAngleModule *module_;

    public:
    AxisAngleModuleWidget(QWidget *parent, AxisAngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AxisAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfGraph_, *angleGraph_, *dAngleGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

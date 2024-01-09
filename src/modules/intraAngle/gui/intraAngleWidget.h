// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/intraAngle/gui/ui_intraAngleWidget.h"
#include "modules/widget.h"

// Forward Declarations
class IntraAngleModule;
class DataViewer;

// Module Widget
class IntraAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    IntraAngleModule *module_;

    public:
    IntraAngleModuleWidget(QWidget *parent, IntraAngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::IntraAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *angleGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/dAngle/gui/ui_dAngleWidget.h"
#include "modules/widget.h"

// Forward Declarations
class DAngleModule;
class DataViewer;

// Module Widget
class DAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    DAngleModule *module_;

    public:
    DAngleModuleWidget(QWidget *parent, DAngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::DAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfGraph_, *angleGraph_, *dAngleGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

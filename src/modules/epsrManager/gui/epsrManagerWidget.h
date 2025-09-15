// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "modules/epsrManager/gui/ui_epsrManagerWidget.h"
#include "modules/widget.h"

// Forward Declarations
class EPSRManagerModule;
class Dissolve;
class Module;

// Module Widget
class EPSRManagerModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EPSRManagerModuleWidget(QWidget *parent, EPSRManagerModule *module, Dissolve &dissolve);
    ~EPSRManagerModuleWidget() override = default;

    private:
    // Associated Module
    EPSRManagerModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EPSRManagerModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *graph_;

    /*
     * Widgets / Functions
     */
    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    private Q_SLOTS:
    void on_PotentialsButton_clicked(bool checked);
    void on_RFactorButton_clicked(bool checked);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "modules/refine/gui/ui_refinewidget.h"
#include "modules/widget.h"

// Forward Declarations
class DataViewer;
class Dissolve;
class Module;
class PartialSet;
class RefineModule;

// Module Widget
class RefineModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    RefineModuleWidget(QWidget *parent, RefineModule *module, Dissolve &dissolve);
    ~RefineModuleWidget() override = default;

    private:
    // Associated Module
    RefineModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::RefineModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *graph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private slots:
    void on_TotalFQButton_clicked(bool checked);
    void on_DeltaFQButton_clicked(bool checked);
    void on_EstimatedSQButton_clicked(bool checked);
    void on_EstimatedGRButton_clicked(bool checked);
    void on_TotalGRButton_clicked(bool checked);
    void on_PotentialsButton_clicked(bool checked);
    void on_RFactorButton_clicked(bool checked);
    void on_EReqButton_clicked(bool checked);
};

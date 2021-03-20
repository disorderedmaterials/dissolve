// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/epsr/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSet;
class EPSRModule;
class DataViewer;

// Module Widget
class EPSRModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EPSRModuleWidget(QWidget *parent, const GenericList &processingData, EPSRModule *module, Dissolve &dissolve);
    ~EPSRModuleWidget();

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Associated Module
    EPSRModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EPSRModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *graph_;

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;

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

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/epsr/gui/ui_epsrWidget.h"
#include "modules/widget.h"

// Forward Declarations
class DataViewer;
class Dissolve;
class EPSRModule;
class Module;
class PartialSet;

// Module Widget
class EPSRModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EPSRModuleWidget(QWidget *parent, EPSRModule *module, Dissolve &dissolve);
    ~EPSRModuleWidget() override = default;

    private:
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
    QList<QAction *> rangeActionsList;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */

    private Q_SLOTS:
    void on_TotalFQButton_clicked(bool checked);
    void on_DeltaFQButton_clicked(bool checked);
    void on_EstimatedSQButton_clicked(bool checked);
    void on_EstimatedGRButton_clicked(bool checked);
    void on_TotalGRButton_clicked(bool checked);
    void on_PotentialsButton_clicked(bool checked);
    void on_RFactorButton_clicked(bool checked);
    void on_EReqButton_clicked(bool checked);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/accumulate/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSetAccumulator;
class AccumulateModule;
class DataViewer;

// Module Widget
class AccumulateModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AccumulateModuleWidget(QWidget *parent, const GenericList &processingData, AccumulateModule *module, Dissolve &dissolve);
    ~AccumulateModuleWidget();

    private:
    // Associated Module
    AccumulateModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSetAccumulator> targetPartials_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AccumulateModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *graph_;

    private:
    // Create renderables for current target PartialSet
    void createPartialSetRenderables(std::string_view targetPrefix);

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private slots:
    void on_PartialsButton_clicked(bool checked);
    void on_TotalButton_clicked(bool checked);
};

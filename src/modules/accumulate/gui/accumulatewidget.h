// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "modules/accumulate/gui/ui_accumulatewidget.h"
#include "modules/widget.h"

// Forward Declarations
class AccumulateModule;
class DataViewer;
class Dissolve;
class Module;
class PartialSetAccumulator;

// Module Widget
class AccumulateModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AccumulateModuleWidget(QWidget *parent, AccumulateModule *module, Dissolve &dissolve);
    ~AccumulateModuleWidget() override = default;

    private:
    // Associated Module
    AccumulateModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSetAccumulator> targetPartials_;

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
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private slots:
    void on_PartialsButton_clicked(bool checked);
    void on_TotalButton_clicked(bool checked);
};

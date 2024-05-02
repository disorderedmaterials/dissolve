// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/gr/gui/ui_grWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class GRModule;
class PartialSet;
class DataViewer;

// Module Widget
class GRModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    GRModuleWidget(QWidget *parent, GRModule *module, Dissolve &dissolve);
    ~GRModuleWidget() override = default;

    private:
    // Associated Module
    GRModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::GRModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_;

    private:
    // Create renderables for current target PartialSet
    void createPartialSetRenderables(std::string_view targetPrefix);

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private Q_SLOTS:
    void on_SummedPartialsButton_clicked(bool checked);
    void on_TotalsButton_clicked(bool checked);
    void on_ConfigurationPartialsButton_clicked(bool checked);
    void on_ConfigurationTargetCombo_currentIndexChanged(int index);
    void on_FilterEdit_textChanged(QString text);
    void on_ClearFilterButton_clicked(bool checked);
};

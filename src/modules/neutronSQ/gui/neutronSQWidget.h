// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/neutronSQ/gui/ui_neutronSQWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Dissolve;
class NeutronSQModule;
class PartialSet;
class DataViewer;

// Module Widget
class NeutronSQModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    NeutronSQModuleWidget(QWidget *parent, NeutronSQModule *module, Dissolve &dissolve);
    ~NeutronSQModuleWidget() override = default;

    private:
    // Associated Module
    NeutronSQModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::NeutronSQModuleWidget ui_;
    // DataViewers contained within this widget
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
    private Q_SLOTS:
    void on_TotalFQButton_clicked(bool checked);
    void on_PartialSQButton_clicked(bool checked);
    void on_TotalGRButton_clicked(bool checked);
    void on_PartialGRButton_clicked(bool checked);
    void on_FilterEdit_textChanged(QString text);
    void on_ClearFilterButton_clicked(bool checked);
};

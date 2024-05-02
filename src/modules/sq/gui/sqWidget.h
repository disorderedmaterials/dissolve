// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/sq/gui/ui_sqWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSet;
class SQModule;
class DataViewer;

// Module Widget
class SQModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    SQModuleWidget(QWidget *parent, SQModule *module, Dissolve &dissolve);
    ~SQModuleWidget() override = default;

    private:
    // Associated Module
    SQModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SQModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *sqGraph_;

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
    void on_PartialsButton_clicked(bool checked);
    void on_TotalButton_clicked(bool checked);
    void on_FilterEdit_textChanged(QString text);
    void on_ClearFilterButton_clicked(bool checked);
};

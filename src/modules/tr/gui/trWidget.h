// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/tr/gui/ui_trWidget.h"
#include "modules/widget.h"

// Forward Declarations
class Dissolve;
class Module;
class TRModule;
class PartialSet;
class DataViewer;

// Module Widget
class TRModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    TRModuleWidget(QWidget *parent, TRModule *module, Dissolve &dissolve);
    ~TRModuleWidget() override = default;

    private:
    // Associated Module
    TRModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::TRModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *trGraph_;

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

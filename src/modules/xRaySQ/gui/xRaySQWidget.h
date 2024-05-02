// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/widget.h"
#include "modules/xRaySQ/gui/ui_xRaySQWidget.h"

// Forward Declarations
class Dissolve;
class XRaySQModule;
class PartialSet;
class DataViewer;

// Module Widget
class XRaySQModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    XRaySQModuleWidget(QWidget *parent, XRaySQModule *module, Dissolve &dissolve);
    ~XRaySQModuleWidget() override = default;

    private:
    // Associated Module
    XRaySQModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::XRaySQModuleWidget ui_;
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
    private Q_SLOTS:
    void on_TotalFQButton_clicked(bool checked);
    void on_PartialSQButton_clicked(bool checked);
    void on_TotalGRButton_clicked(bool checked);
    void on_PartialGRButton_clicked(bool checked);
    void on_FilterEdit_textChanged(QString text);
    void on_ClearFilterButton_clicked(bool checked);
};

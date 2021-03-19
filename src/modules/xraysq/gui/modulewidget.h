// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/xraysq/gui/ui_modulewidget.h"

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
    XRaySQModuleWidget(QWidget *parent, const GenericList &processingData, XRaySQModule *module, Dissolve &dissolve);
    ~XRaySQModuleWidget();

    private:
    // Associated Module
    XRaySQModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;
    // Reference to Dissolve
    Dissolve &dissolve_;

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
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(XRaySQModule *module);

    private slots:
    void on_TotalFQButton_clicked(bool checked);
    void on_PartialSQButton_clicked(bool checked);
    void on_TotalGRButton_clicked(bool checked);
    void on_PartialGRButton_clicked(bool checked);
};

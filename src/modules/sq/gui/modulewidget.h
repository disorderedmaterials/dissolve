// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/sq/gui/ui_modulewidget.h"

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
    SQModuleWidget(QWidget *parent, const GenericList &processingData, SQModule *module, Dissolve &dissolve);
    ~SQModuleWidget();

    private:
    // Associated Module
    SQModule *module_;
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;
    // Reference to Dissolve
    Dissolve &dissolve_;

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
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private slots:
    void on_PartialsButton_clicked(bool checked);
    void on_TotalButton_clicked(bool checked);
};

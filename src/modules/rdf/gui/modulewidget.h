// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/rdf/gui/ui_modulewidget.h"

// Forward Declarations
class Configuration;
class Dissolve;
class Module;
class RDFModule;
class PartialSet;
class DataViewer;

// Module Widget
class RDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    RDFModuleWidget(QWidget *parent, const GenericList &processingData, RDFModule *module, Dissolve &dissolve);
    ~RDFModuleWidget();

    private:
    // Associated Module
    RDFModule *module_;
    // DataViewers contained within this widget
    DataViewer *partialsGraph_, *totalsGraph_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::RDFModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;

    /*
     * Widgets / Functions
     */
    private:
    // Current Configuration whose data is being displayed
    Configuration *currentConfiguration_;

    private:
    // Set data targets in graphs
    void setGraphDataTargets(RDFModule *module);

    private slots:
    void on_TargetCombo_currentIndexChanged(int index);
};

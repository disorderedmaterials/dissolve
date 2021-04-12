// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/helpers/comboboxcontroller.h"
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
    // Target partial data being displayed (if any)
    OptionalReferenceWrapper<const PartialSet> targetPartials_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::RDFModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_;

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
    void on_SummedPartialsButton_clicked(bool checked);
    void on_TotalsButton_clicked(bool checked);
    void on_ConfigurationPartialsButton_clicked(bool checked);
    void on_ConfigurationTargetCombo_currentIndexChanged(int index);
};

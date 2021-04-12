// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_rdf/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateRDFModule;
class DataViewer;

// Module Widget
class CalculateRDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    CalculateRDFModule *module_;

    public:
    CalculateRDFModuleWidget(QWidget *parent, const GenericList &processingData, CalculateRDFModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateRDFModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_;

    public:
    // Update controls within widget
    void updateControls(ModuleWidget::UpdateType updateType) override;
};

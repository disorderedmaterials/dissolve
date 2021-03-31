// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_cn/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateCNModule;

// Module Widget
class CalculateCNModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    CalculateCNModuleWidget(QWidget *parent, const GenericList &processingData, CalculateCNModule *cnModule);

    /*
     * Data
     */
    private:
    // Associated Module
    CalculateCNModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateCNModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * Widgets / Functions
     */
    private:
    // DataViewer for RDF plot
    DataViewer *rdfGraph_;
    // Whether the RDF data to plot has been succesfully located
    bool rdfDataLocated_;

    private:
    // Set data targets in graphs
    void setGraphDataTargets();

    private slots:
};

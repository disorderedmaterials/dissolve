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
    // DataViewers contained within this widget
    DataViewer *partialsGraph_, *totalGraph_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SQModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(SQModule *module);
};

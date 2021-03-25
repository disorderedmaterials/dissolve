// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/neutronsq/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class NeutronSQModule;
class PartialSet;
class DataViewer;

// Module Widget
class NeutronSQModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    NeutronSQModuleWidget(QWidget *parent, const GenericList &processingData, NeutronSQModule *module, Dissolve &dissolve);
    ~NeutronSQModuleWidget();

    private:
    // Associated Module
    NeutronSQModule *module_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::NeutronSQModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *partialGRGraph_, *partialSQGraph_, *totalGRGraph_, *totalFQGraph_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(NeutronSQModule *module);
};

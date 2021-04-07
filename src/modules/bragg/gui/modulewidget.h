// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/bragg/gui/ui_modulewidget.h"

// Forward Declarations
class BraggModule;
class Configuration;
class Dissolve;
class Module;
class PartialSet;
class DataViewer;

// Module Widget
class BraggModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    BraggModuleWidget(QWidget *parent, const GenericList &processingData, BraggModule *module);
    ~BraggModuleWidget();

    private:
    // Associated Module
    BraggModule *module_;
    // DataViewers contained within this widget
    DataViewer *reflectionsGraph_, *totalsGraph_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::BraggModuleWidget ui_;

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
    void setGraphDataTargets();

    private slots:
    void on_TargetCombo_currentIndexChanged(int index);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/epsr/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSet;
class EPSRModule;
class DataViewer;

// Module Widget
class EPSRModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    EPSRModuleWidget(QWidget *parent, EPSRModule *module, Dissolve &dissolve);
    ~EPSRModuleWidget();

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Associated Module
    EPSRModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EPSRModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *FQGraph_, *FQFitGraph_, *estimatedSQGraph_, *estimatedGRGraph_, *totalGRGraph_, *phiRGraph_, *phiMagGraph_,
        *rFactorGraph_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * State I/O
     */
    public:
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
    // Read widget state through specified LineParser
    bool readState(LineParser &parser);

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(EPSRModule *module);

    /*
     * Debug Tab
     */
    private:
    // Temporary data currently shown on debug tab
    List<Data1D> debugFunctionData_;

    private:
    // Update data shown on EP functions viewer
    void updateDebugEPFunctionsGraph(int from, int to);

    private slots:
    void on_DebugFromSpin_valueChanged(int value);
    void on_DebugToSpin_valueChanged(int value);
};

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/refine/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSet;
class RefineModule;
class DataViewer;

// Module Widget
class RefineModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    RefineModuleWidget(QWidget *parent, RefineModule *module, Dissolve &dissolve);
    ~RefineModuleWidget();

    private:
    // Associated Module
    RefineModule *module_;
    // DataViewers contained within this widget
    DataViewer *dataGraph_, *partialSQGraph_, *partialGRGraph_, *deltaPhiRGraph_, *phiMagGraph_, *errorsGraph_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::RefineModuleWidget ui_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * State I/O
     */
    public:
    // Read widget state through specified LineParser
    bool readState(LineParser &parser);
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(RefineModule *module);
};

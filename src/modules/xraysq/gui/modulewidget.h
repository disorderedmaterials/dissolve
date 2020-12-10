// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/xraysq/gui/ui_modulewidget.h"

// Forward Declarations
class Dissolve;
class XRaySQModule;
class PartialSet;
class DataViewer;

// Module Widget
class XRaySQModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    XRaySQModuleWidget(QWidget *parent, XRaySQModule *module, Dissolve &dissolve);
    ~XRaySQModuleWidget();

    private:
    // Associated Module
    XRaySQModule *module_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::XRaySQModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *partialGRGraph_, *partialSQGraph_, *totalGRGraph_, *totalFQGraph_;

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
    void setGraphDataTargets(XRaySQModule *module);
};

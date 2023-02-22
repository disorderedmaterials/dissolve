// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "modules/intraangle/gui/ui_intraanglewidget.h"
#include "modules/widget.h"

// Forward Declarations
class IntraAngleModule;
class DataViewer;

// Module Widget
class IntraAngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    IntraAngleModule *module_;

    public:
    IntraAngleModuleWidget(QWidget *parent, IntraAngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::IntraAngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfABGraph_, *rdfBCGraph_, *angleGraph_, *dAngleABGraph_, *dAngleBCGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets(IntraAngleModule *module);

    private slots:
};

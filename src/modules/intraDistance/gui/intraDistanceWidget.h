// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/intraDistance/gui/ui_intraDistanceWidget.h"
#include "modules/widget.h"

// Forward Declarations
class IntraDistanceModule;
class DataViewer;

// Module Widget
class IntraDistanceModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    IntraDistanceModule *module_;

    public:
    IntraDistanceModuleWidget(QWidget *parent, IntraDistanceModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::IntraDistanceModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

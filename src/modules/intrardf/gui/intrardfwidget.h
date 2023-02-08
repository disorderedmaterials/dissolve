// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "modules/intrardf/gui/ui_intrardfwidget.h"
#include "modules/widget.h"

// Forward Declarations
class IntraRDFModule;
class DataViewer;

// Module Widget
class IntraRDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    IntraRDFModule *module_;

    public:
    IntraRDFModuleWidget(QWidget *parent, IntraRDFModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::IntraRDFModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

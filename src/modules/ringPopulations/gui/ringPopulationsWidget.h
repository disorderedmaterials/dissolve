// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/ringPopulations/gui/ui_ringPopulationsWidget.h"
#include "modules/widget.h"

// Forward Declarations
class RingPopulationsModule;
class DataViewer;

// Module Widget
class RingPopulationsModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    RingPopulationsModule *module_;

    public:
    RingPopulationsModuleWidget(QWidget *parent, RingPopulationsModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::RingPopulationsModuleWidget ui_;

    // DataViewer contained within this widget
    DataViewer *qSpeciesGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

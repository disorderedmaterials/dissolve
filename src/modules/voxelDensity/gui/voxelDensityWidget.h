// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

//#include "modules/voxelDensityWidget/gui/ui_voxelDensityWidget.h"
#include "modules/widget.h"

// Forward Declarations
class VoxelDensityModule;
class DataViewer;

// Module Widget
class VoxelDensityModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    VoxelDensityModule *module_;

    public:
    VoxelDensityModuleWidget(QWidget *parent, VoxelDensityModule *module, Dissolve &dissolve) {}

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::VoxelDensityModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfGraph_, *angleGraph_, *dAngleGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

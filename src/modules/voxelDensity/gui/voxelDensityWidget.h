// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/voxelDensity/voxelDensity.h"
#include "modules/voxelDensityWidget/gui/ui_voxelDensityWidget.h"
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
    VoxelDensityModuleWidget(QWidget *parent, VoxelDensityModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::VoxelDensityModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *voxelDensityGraph_;
    // Get the horizontal axis label for the module 1D data
    std::optional<std::string> getData1DAxisLabel();

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};

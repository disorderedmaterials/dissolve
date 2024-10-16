// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "gui/render/renderableData2D.h"
#include "main/dissolve.h"
#include "modules/voxelDensity/gui/voxelDensityWidget.h"
#include <cmath>

VoxelDensityModuleWidget::VoxelDensityModuleWidget(QWidget *parent, VoxelDensityModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up voxel density graph
    voxelDensityGraph_ = ui_.VoxelDensityPlotWidget->dataViewer();
    // -- Set view
    voxelDensityGraph_->view().setViewType(View::FlatXYView);
    voxelDensityGraph_->view().axes().setTitle(0, getData1DAxisLabel().value_or(""));
    voxelDensityGraph_->view().axes().setMax(0, 10.0);
    voxelDensityGraph_->view().axes().setTitle(1, "N");
    voxelDensityGraph_->view().axes().setMin(1, -1.0);
    voxelDensityGraph_->view().axes().setMax(1, 1.0);
    voxelDensityGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    voxelDensityGraph_->view().setAutoFollowType(View::AllAutoFollow);
}

// Get the horizontal axis label for the module 1D data
std::optional<std::string> VoxelDensityModuleWidget::getData1DAxisLabel()
{
    auto type = module_->getCurrentProperty();
    switch (type)
    {
        case (VoxelDensityModule::TargetPropertyType::Mass):
            return "Atomic mass unit, u \\sym{angstrom}\\sup{-3}";
        case (VoxelDensityModule::TargetPropertyType::AtomicNumber):
            return "Z \\sym{angstrom}\\sup{-3}";
        case (VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity):
            return "Scattering length density, cm\\sup{-2} \\sym{angstrom}\\sup{-3}";
        default:
            throw(std::runtime_error(fmt::format("'{}' not a valid property.\n", static_cast<int>(type))));
    }
}

/*
 * UI
 */

// Update controls within widget
void VoxelDensityModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
        voxelDensityGraph_->clearRenderables();

    if (voxelDensityGraph_->renderables().empty())
    {
        auto *cfg = module_->keywords().getConfiguration("Configuration");
        if (cfg)
            voxelDensityGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Data1D", module_->name()),
                                                                   fmt::format("Data1D//{}", cfg->niceName()), cfg->niceName());
    }

    voxelDensityGraph_->view().axes().setTitle(0, getData1DAxisLabel().value_or(""));

    if (module_)
    {
        auto voxelVolume = std::ceil(module_->voxelVolume() * std::pow(10.0, 3)) / std::pow(10, 3);
        std::string yAxisTitle =
            "N Voxels [of volume=" + std::string(fmt::format("{}", voxelVolume)) + " \\sym{angstrom}\\sup{3}" + "]";
        voxelDensityGraph_->view().axes().setTitle(1, yAxisTitle);
    }

    // Validate renderables if they need it
    voxelDensityGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.VoxelDensityPlotWidget->updateToolbar();
    voxelDensityGraph_->postRedisplay();
}

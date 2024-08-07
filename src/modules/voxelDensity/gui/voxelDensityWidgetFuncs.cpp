// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "gui/render/renderableData1D.h"
#include "gui/render/renderableData2D.h"
#include "main/dissolve.h"
#include "modules/voxelDensity/gui/voxelDensityWidget.h"


VoxelDensityModuleWidget::VoxelDensityModuleWidget(QWidget *parent, VoxelDensityModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up voxel density graph
    voxelDensityGraph_ = ui_.RDFPlotWidget->dataViewer();
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

    // Validate renderables if they need it
    voxelDensityGraph_->validateRenderables(dissolve_.processingModuleData());

    ui_.voxelDensityPlotWidget->updateToolbar();
    voxelDensityGraph_->postRedisplay();
}

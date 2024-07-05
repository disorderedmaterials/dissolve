#include "module/context.h"
#include "voxelDensity.h"
#include "math/histogram3D.h"
#include "classes/region.h"
#include "procedure/nodes/customRegion.h"

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Voxelise unit cell
    Region unitCellVoxels;
    auto anyFreeVoxels = unitCellVoxels.generate(targetConfiguration_, 1.0, false, []() { 
        return std::make_shared<CustomRegionVoxelKernel>(); 
    });

    // Calculate target property density
    auto [hist, status] = processingData.realiseIf<Histogram3D>("Histo-Voxels", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        hist.initialise(xAxisRange_.x, xAxisRange_.y, xAxisRange_.y,
                        yAxisRange_.x, yAxisRange_.y, yAxisRange_.z, 
                        zAxisRange_.x, zAxisRange_.y, zAxisRange_.z);
    hist.zeroBins();



}
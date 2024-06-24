// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"

VoxelDensityModule::VoxelDensityModule() : Module(ModuleTypes::VoxelDensity)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Voxel density");

    executeIfTargetsUnchanged_ = true;
}

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &moduleContext) 
{
    return ExecutionResult::NotExecuted;
}
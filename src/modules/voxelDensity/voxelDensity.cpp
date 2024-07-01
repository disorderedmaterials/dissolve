// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesVector.h"
#include "keywords/vec3Double.h"

VoxelDensityModule::VoxelDensityModule() : Module(ModuleTypes::VoxelDensity)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Voxel Region");
    keywords_.add<Vec3DoubleKeyword>("UnitCellVoxelRegion", "3D bounds for the region within the voxelised unit cell", unitCellVoxelRegion_,
                                    Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.setOrganisation("Options", "Species");
    keywords_.add<SpeciesVectorKeyword>("SpeciesRange", "Constrain species for analysis", restrictToSpecies_);
    keywords_.add<EnumOptionsKeyword<VoxelDensityModule::TargetPropertyType>>("TargetProperty", 
                                    "Target property for analysis", targetProperty_, VoxelDensityModule::targetPropertyTypes());
}

// Return enum option info for TargetPropertyType
EnumOptions<VoxelDensityModule::TargetPropertyType> VoxelDensityModule::targetPropertyTypes()
{
    return EnumOptions<VoxelDensityModule::TargetPropertyType>(
        "TargetPropertyType", {
            {TargetPropertyType::Mass, "Mass"},
            {TargetPropertyType::AtomicNumber, "AtomicNumber"},
            {TargetPropertyType::ScatteringLengthDensity, "ScatteringLengthDensity"}
        }
    );
}

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &moduleContext) 
{
    return ExecutionResult::NotExecuted;
}
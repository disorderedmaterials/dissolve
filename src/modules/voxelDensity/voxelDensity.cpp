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
                                    targetConfiguration_->box()->axisLengths(), std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Species");
    keywords_.add<SpeciesVectorKeyword>("SpeciesRange", "Constrain species for analysis", restrictToSpecies_);

    keywords_.setOrganisation("Options", "Atomic Number");
    keywords_.add<BoolKeyword>("AtomicNumberSelected", "Atomic number distribution by voxel", atomicNumberSelected_);

    keywords_.setOrganisation("Options", "Mass");
    keywords_.add<BoolKeyword>("MassSelected", "Mass distribution by voxel", massSelected_);

    keywords_.setOrganisation("Options", "Scattering Length Density");
    keywords_.add<BoolKeyword>("ScatteringLengthDensitySelected", "Scattering length density (SLD) distribution by voxel", scatteringLengthDensitySelected_);
}

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &moduleContext) 
{
    return ExecutionResult::NotExecuted;
}
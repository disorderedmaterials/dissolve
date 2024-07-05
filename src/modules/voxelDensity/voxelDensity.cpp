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

    keywords_.setOrganisation("Options", "Species");
    keywords_.add<SpeciesVectorKeyword>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("RangeX", "Range (min, max, binwidth) of X distance binning", xAxisRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeY", "Range (min, max, binwidth) of Y distance binning", yAxisRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeZ", "Range (min, max, binwidth) of Z distance binning", zAxisRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "TargetProperty", "Target property for analysis");
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
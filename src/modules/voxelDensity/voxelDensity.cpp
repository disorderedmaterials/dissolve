// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/integer.h"
#include "keywords/speciesVector.h"
#include "keywords/vec3Double.h"

VoxelDensityModule::VoxelDensityModule() : Module(ModuleTypes::VoxelDensity)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Species");
    keywords_.add<SpeciesVectorKeyword>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    keywords_.setOrganisation("Options", "Analysis", "Set the voxel density and property for analysis");
    keywords_.add<IntegerKeyword>("NumberOfPoints", "Number of points over which analysis is performed", numPoints_, 1);
    keywords_.add<EnumOptionsKeyword<VoxelDensityModule::TargetPropertyType>>(
        "TargetProperty", "Target property for analysis", targetProperty_, VoxelDensityModule::targetPropertyTypes());

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated Histogram1D data",
                                        exportFileAndFormat_, "EndExport");
}

// Return enum option info for TargetPropertyType
EnumOptions<VoxelDensityModule::TargetPropertyType> VoxelDensityModule::targetPropertyTypes()
{
    return EnumOptions<VoxelDensityModule::TargetPropertyType>(
        "TargetPropertyType", {{TargetPropertyType::Mass, "Mass"},
                               {TargetPropertyType::AtomicNumber, "AtomicNumber"},
                               {TargetPropertyType::ScatteringLengthDensity, "ScatteringLengthDensity"}});
}
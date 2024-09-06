// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileAndFormat.h"
#include "keywords/integer.h"
#include "keywords/speciesVector.h"
#include "keywords/vec3Double.h"

VoxelDensityModule::VoxelDensityModule() : Module(ModuleTypes::VoxelDensity)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("BinRange", "Range (min, max, delta) of voxel axis", binRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Analysis", "Set the voxel density and property for analysis");
    keywords_.add<DoubleKeyword>("VoxelSideLength", "Side length of a single analysis voxel (angstroms)", idealVoxelSideLength_,
                                 0.1);
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

// Target property for analysis
VoxelDensityModule::TargetPropertyType VoxelDensityModule::getCurrentProperty() const { return targetProperty_; }

// Voxel volume (cubic angstroms)
double VoxelDensityModule::voxelVolume() const { return voxelVolume_; }

// Actual side length of a single analysis voxel (angstroms), calculated to suit the given unit cell axis
double VoxelDensityModule::voxelSideLength(const double axisLength) const
{
    return axisLength / round(axisLength / idealVoxelSideLength_);
}

// Return bound-coherent natural isotope scattering length density for element
double VoxelDensityModule::scatteringLengthDensity(Elements::Element Z)
{
    return Sears91::boundCoherent(Sears91::naturalIsotope(Z));
}
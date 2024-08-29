// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "data/atomicMasses.h"
#include "data/isotopes.h"
#include "io/export/data1D.h"
#include "module/module.h"
#include "templates/array3D.h"
#include <cmath>

// Forward declarations

class Configuration;

// VoxelDensity Module
class VoxelDensityModule : public Module
{
    public:
    VoxelDensityModule();
    ~VoxelDensityModule() override = default;

    /*
     * Definition
     */
    public:
    // Target property
    enum class TargetPropertyType
    {
        Mass,
        AtomicNumber,
        ScatteringLengthDensity
    };
    // Return enum option info for TargetPropertyType
    static EnumOptions<VoxelDensityModule::TargetPropertyType> targetPropertyTypes();
    // Target property for analysis
    VoxelDensityModule::TargetPropertyType getCurrentProperty() { return targetProperty_; }
    // Voxel volume (cubic angstroms)
    const double voxelVolume() { return voxelVolume_; }

    private:
    // Range (min, max, delta) of voxel histogram axis
    Vec3<double> binRange_{0.0, 10.0, 0.05};
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Vector of species by which analysis is restricted
    std::vector<const Species *> restrictToSpecies_;
    // Target property for analysis
    VoxelDensityModule::TargetPropertyType targetProperty_{TargetPropertyType::Mass};
    // Requested ideal side length of a single analysis voxel (angstroms)
    double idealVoxelSideLength_{1.0};
    // Number of analysis points along each axis of the unit cell
    Vec3<int> nAxisVoxels_{1, 1, 1};
    // Voxel volume (cubic angstroms)
    double voxelVolume_{1.0};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Add value to array
    void addValue(Vec3<double> coords, double value, Array3D<double> &array);
    // Return atomic coordinates folded into unit cell
    Vec3<double> foldedCoordinates(const Vec3<double> &r, const Box *unitCell);
    // Return bound-coherent natural isotope scattering length density for element
    double scatteringLengthDensity(Elements::Element Z) { return Sears91::boundCoherent(Sears91::naturalIsotope(Z)); }
    // Actual side length of a single analysis voxel (angstroms), calculated to suit the given unit cell axis
    const double voxelSideLength(const double axisLength) { return axisLength / round(axisLength / idealVoxelSideLength_); }
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
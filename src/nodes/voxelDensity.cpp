// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/voxelDensity.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "data/atomicMasses.h"

VoxelDensityNode::VoxelDensityNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("Range", "Range (min, max, delta) of binned property data", binRange_);
    addOption("VoxelSideLength", "Requested side length of a single analysis voxel (Angstroms)", requestedVoxelSideLength_);
    addOption("TargetProperty", "Target property for analysis", targetProperty_);
}

std::string_view VoxelDensityNode::type() const { return "VoxelDensity"; }

std::string_view VoxelDensityNode::summary() const
{
    return "Describe distribution of atomic number, mass, and scattering length density across unit cell voxels";
}

/*
 * Definition
 */

// Return enum option info for TargetPropertyType
EnumOptions<VoxelDensityNode::TargetPropertyType> VoxelDensityNode::targetPropertyTypes()
{
    return EnumOptions<VoxelDensityNode::TargetPropertyType>(
        "TargetPropertyType", {{TargetPropertyType::Mass, "Mass"},
                               {TargetPropertyType::AtomicNumber, "AtomicNumber"},
                               {TargetPropertyType::ScatteringLengthDensity, "ScatteringLengthDensity"}});
}

/*
 * Data
 */

// Clear any local data
void VoxelDensityNode::clearData()
{
    voxels_.reset();
    histogram_.reset();
    values_.clear();
}

// Temporary accessors to data for testing
const Array3D<double> &VoxelDensityNode::voxels() const { return *voxels_; }
const Histogram1D &VoxelDensityNode::histogram() const { return *histogram_; };
const Data1D &VoxelDensityNode::values() const { return values_; };

/*
 * Processing
 */

// Target property for analysis
VoxelDensityNode::TargetPropertyType VoxelDensityNode::getCurrentProperty() const { return targetProperty_; }

// Voxel volume (cubic angstroms)
double VoxelDensityNode::voxelVolume() const { return voxelVolume_; }

// Actual side length of a single analysis voxel (angstroms), calculated to suit the given unit cell axis
double VoxelDensityNode::voxelSideLength(const double axisLength) const
{
    return axisLength / round(axisLength / requestedVoxelSideLength_.asDouble());
}

// Add value to array
void VoxelDensityNode::addValue(const Vector3 &coords, double value)
{
    (*voxels_)[{(int)std::floor(coords.x * nAxisVoxels_.x), (int)std::floor(coords.y * nAxisVoxels_.y),
                (int)std::floor(coords.z * nAxisVoxels_.z)}] += value;
}

// Return atomic coordinates folded into unit cell
Vector3 VoxelDensityNode::foldedCoordinates(const Vector3 &r, const Box &unitCell) { return unitCell.foldFrac(r); }

// Run main processing
NodeConstants::ProcessResult VoxelDensityNode::process()
{
    auto unitCell = configuration_->box();

    // Initialise data storage
    if (!voxels_)
    {
        // Define voxels
        const double boxX = unitCell.axisLengths().x, boxY = unitCell.axisLengths().y, boxZ = unitCell.axisLengths().z;
        voxelVolume_ = voxelSideLength(boxX) * voxelSideLength(boxY) * voxelSideLength(boxZ);
        nAxisVoxels_.x = int(round(boxX / voxelSideLength(boxX)));
        nAxisVoxels_.y = int(round(boxY / voxelSideLength(boxY)));
        nAxisVoxels_.z = int(round(boxZ / voxelSideLength(boxZ)));

        message("Volume of unit cell voxels: {}.\n", voxelVolume_);
        message("Number of voxels along each axis: nX={}, nY={}, nZ={}.\n", nAxisVoxels_.x, nAxisVoxels_.y, nAxisVoxels_.z);
        voxels_.emplace().initialise(nAxisVoxels_.x, nAxisVoxels_.y, nAxisVoxels_.z);
    }
    if (!histogram_)
        histogram_.emplace().initialise(binRange_.x, binRange_.y, binRange_.z);

    // Calculate target property 3d map over unit cell voxels
    (*voxels_) = 0.0;
    switch (targetProperty_)
    {
        case TargetPropertyType::Mass:
            for (const auto atom : configuration_->atoms())
                addValue(foldedCoordinates(atom.r(), unitCell), AtomicMass::mass(atom.speciesAtom()->Z()));
            break;
        case TargetPropertyType::AtomicNumber:
            for (const auto atom : configuration_->atoms())
                addValue(foldedCoordinates(atom.r(), unitCell), atom.speciesAtom()->Z());
            break;
        case TargetPropertyType::ScatteringLengthDensity:
            for (const auto atom : configuration_->atoms())
                addValue(foldedCoordinates(atom.r(), unitCell),
                         Sears91::boundCoherent(Sears91::naturalIsotope(atom.speciesAtom()->Z())));
            break;
        default:
            Messenger::exception("'{}' not a valid property.\n", static_cast<int>(targetProperty_));
    }

    // Calculate voxel density histogram, normalising bin values by voxel volume (property/cubic angstrom)
    histogram_->zeroBins();
    for (const auto &value : voxels_->values())
        histogram_->bin(value / voxelVolume_);
    histogram_->accumulate();

    values_ = histogram_->accumulatedData();

    return NodeConstants::ProcessResult::Success;
}

EnumOptions<VoxelDensityNode::TargetPropertyType> getEnumOptions(VoxelDensityNode::TargetPropertyType)
{
    return VoxelDensityNode::targetPropertyTypes();
}

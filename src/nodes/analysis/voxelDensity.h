// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/histogram1D.h"
#include "math/vector3i.h"
#include "nodes/node.h"
#include "templates/array3D.h"

// Forward declarations
class Box;
class Configuration;

class VoxelDensityNode : public Node
{
    public:
    VoxelDensityNode(Graph *parentGraph);
    ~VoxelDensityNode() override = default;

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
    static EnumOptions<VoxelDensityNode::TargetPropertyType> targetPropertyTypes();

    private:
    // Range (min, max, delta) of voxel histogram axis
    Vector3 binRange_{0.0, 10.0, 0.05};
    // Target configuration
    Configuration *configuration_{nullptr};
    // Target property for analysis
    VoxelDensityNode::TargetPropertyType targetProperty_{TargetPropertyType::Mass};
    // Requested side length of a single voxel (Angstroms)
    Number requestedVoxelSideLength_{1.0};
    // Number of analysis points along each axis of the unit cell
    Vector3i nAxisVoxels_{1, 1, 1};
    // Voxel volume (cubic angstroms)
    double voxelVolume_{1.0};

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // 3D array for voxelising unit cell
    std::optional<Array3D<double>> voxels_;
    // Histogram of voxel values
    std::optional<Histogram1D> histogram_;
    Data1D values_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Array3D<double> &voxels() const;
    const Histogram1D &histogram() const;
    const Data1D &values() const;

    /*
     * Processing
     */
    private:
    // Add value to array
    void addValue(const Vector3 &coords, double value);
    // Return atomic coordinates folded into unit cell
    Vector3 foldedCoordinates(const Vector3 &r, const Box &unitCell);
    // Actual side length of a single analysis voxel (angstroms), calculated to suit the given unit cell axis
    double voxelSideLength(double axisLength) const;

    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

EnumOptions<VoxelDensityNode::TargetPropertyType> getEnumOptions(VoxelDensityNode::TargetPropertyType);

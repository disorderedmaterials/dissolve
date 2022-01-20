// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "math/matrix3.h"
#include "procedure/nodes/node.h"
#include "templates/array3d.h"
#include <random>

// Forward Declarations
class Box;

// Region Node Base
class RegionProcedureNodeBase : public ProcedureNode
{
    public:
    RegionProcedureNodeBase(ProcedureNode::NodeType nodeType);
    ~RegionProcedureNodeBase() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Region Data
     */
    private:
    // Box of target Configuration
    const Box *box_;
    // 3D map of available voxels
    Array3D<std::pair<Vec3<int>, bool>> voxelMap_;
    // Number of voxels along each cell axis
    Vec3<int> nVoxels_;
    // Fractional voxel size
    Vec3<double> voxelSizeFrac_;
    // Lower-left corner voxel indices of free regions
    std::vector<std::pair<Vec3<int>, bool>> freeVoxels_;

    protected:
    // Return whether voxel centred at supplied real coordinates is valid
    virtual bool isVoxelValid(const Configuration *cfg, const Vec3<double> &vCentre) const = 0;

    public:
    // Return random fractional coordinate inside region
    Vec3<double> randomFractionalCoordinate() const;
    // Return whether specified coordinate is inside a valid voxel of the region
    bool validFractionalCoordinate(const Vec3<double> &rFrac) const;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

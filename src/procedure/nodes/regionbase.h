// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/region.h"
#include "math/matrix3.h"
#include "procedure/nodes/node.h"
#include "templates/array3d.h"
#include <random>

// Forward Declarations
class Box;
class Configuration;

// Region Node Base
class RegionProcedureNodeBase : public ProcedureNode
{
    public:
    explicit RegionProcedureNodeBase(ProcedureNode::NodeType nodeType);
    ~RegionProcedureNodeBase() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Region Data
     */
    private:
    // Invert the logic used to determine free space in the region
    bool invert_{false};
    // Region object
    Region region_;
    // Voxel size (Angstroms)
    double voxelSize_{1.0};

    protected:
    // Return whether voxel centred at supplied real coordinates is valid
    virtual bool isVoxelValid(const Configuration *cfg, const Vec3<double> &r) const = 0;

    public:
    // Return current region data
    const Region &region() const;
    // Generate and return region data based on the supplied configuration
    const Region generateRegion(const Configuration *cfg) const;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

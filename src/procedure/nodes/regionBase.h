// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

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

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
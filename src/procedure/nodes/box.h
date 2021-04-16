// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/array.h"

// Forward Declarations
/* none */

// Box Node
class BoxProcedureNode : public ProcedureNode
{
    public:
    BoxProcedureNode(Vec3<double> lengths = Vec3<double>(1.0, 1.0, 1.0), Vec3<double> angles = Vec3<double>(90, 90, 90),
                     bool nonPeriodic = false);
    ~BoxProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

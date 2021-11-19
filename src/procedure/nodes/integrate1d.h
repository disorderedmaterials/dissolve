// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "math/sampleddouble.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;

// Procedure Node - Integrate1D
class Integrate1DProcedureNode : public ProcedureNode
{
    public:
    Integrate1DProcedureNode(Process1DProcedureNode *target = nullptr);
    ~Integrate1DProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Data
     */
    private:
    // Process1D node that we are targetting
    const Process1DProcedureNode *sourceData_;
    // Ranges for integration regions
    Range range_[3] = {{0.0, 3.0}, {3.0, 6.0}, {6.0, 9.0}};
    // Calculated integral (stored in processing data list)
    SampledDouble integral_[3];

    public:
    // Return calculated integral specified
    const SampledDouble &integral(int index) const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

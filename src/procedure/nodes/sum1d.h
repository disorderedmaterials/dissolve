// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "math/sampleddouble.h"
#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Sum1D
class Sum1DProcedureNode : public ProcedureNode
{
    public:
    Sum1DProcedureNode(const Process1DProcedureNode *target = nullptr);
    ~Sum1DProcedureNode() override = default;

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
    // Process1D node that we are targetting (retrieved from keyword 'SourceData')
    const Process1DProcedureNode *processNode_;
    // Ranges for sums (retrieved from keywords)
    Range rangeA_, rangeB_, rangeC_;
    // Flags for second and third ranges
    bool rangeBEnabled_, rangeCEnabled_;
    // Calculated sums (stored in processing data list)
    SampledDouble sum_[3];

    public:
    // Return calculated sum specified
    const SampledDouble &sum(int index) const;
    // Return whether range B is enabled (from keyword data)
    bool isRangeBEnabled() const;
    // Return whether range C is enabled (from keyword data)
    bool isRangeCEnabled() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};

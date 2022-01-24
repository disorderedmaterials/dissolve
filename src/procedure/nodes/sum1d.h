// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "math/sampleddouble.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;

// Procedure Node - Sum1D
class Sum1DProcedureNode : public ProcedureNode
{
    public:
    Sum1DProcedureNode(std::shared_ptr<Process1DProcedureNode> target = nullptr);
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
    // Process1D node that we are targetting
    std::shared_ptr<const Process1DProcedureNode> sourceData_{nullptr};
    // Ranges for sums
    Range range_[3] = {{0.0, 3.0}, {3.0, 6.0}, {6.0, 9.0}};
    // Flags for ranges
    bool rangeEnabled_[3] = {true, false, false};
    // Calculated sums (stored in processing data list)
    OptionalReferenceWrapper<SampledDouble> sum_[3];

    public:
    // Return calculated sum specified
    SampledDouble sum(int index) const;
    // Return reference to specified range for calculation
    Range &range(int n);
    // Return reference to range enabled flag
    bool &rangeEnabled(int n);
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

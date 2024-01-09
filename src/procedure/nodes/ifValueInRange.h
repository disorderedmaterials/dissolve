// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"
#include <memory>
#include <set>

// IfValueInRange Node
class IfValueInRangeProcedureNode : public ProcedureNode
{
    public:
    explicit IfValueInRangeProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Parameters
     */
    private:
    // Pointers to individual parameters
    std::shared_ptr<ExpressionVariable> currentValueParameter_;

    /*
     * Value & Acceptable Range
     */
    private:
    // Value that we are testing
    NodeValue value_;
    // Range of acceptable values
    Range valueRange_{0.0, 5.0};

    /*
     * Branch
     */
    private:
    // Branch for Then
    ProcedureNodeSequence thenBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<ProcedureNodeSequence> branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};

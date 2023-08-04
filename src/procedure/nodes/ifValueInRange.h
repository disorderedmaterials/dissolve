// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
    // Defined parameters
    std::vector<std::shared_ptr<ExpressionVariable>> parameters_;
    // Pointers to individual parameters
    std::shared_ptr<ExpressionVariable> currentValueParameter_;

    public:
    // Add new parameter
    std::shared_ptr<ExpressionVariable> addParameter(std::string_view name, ExpressionValue initialValue);
    // Return the named parameter (if it exists)
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter) override;
    // Return vector of all parameters for this node
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> parameters() const override;

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

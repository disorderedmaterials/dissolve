// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"
#include "keywords/integer.h"


// Iterate For Node
class IterateForProcedureNode : public ProcedureNode
{
    public:
    explicit IterateForProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Counter
     */
    private:
    // Number of iterations to perform
    int nIterations_{100};
    // IntegerKeyword nIterations_;

    /*
     * Branch
     */
    private:
    // Branch to iterate
    ProcedureNodeSequence iterateBranch_;

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
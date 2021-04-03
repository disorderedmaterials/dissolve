// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/sampleddouble.h"
#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Integrate1D
class Integrate1DProcedureNode : public ProcedureNode
{
    public:
    Integrate1DProcedureNode(const Process1DProcedureNode *target = nullptr);
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
    // Process1D node that we are targetting (retrieved from keyword 'SourceData')
    const Process1DProcedureNode *processNode_;
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

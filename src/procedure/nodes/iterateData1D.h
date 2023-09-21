// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Collect1DProcedureNode;
class Data1D;
class IntegerCollect1DProcedureNode;
class LineParser;

// Procedure Node - IterateData1D
class IterateData1DProcedureNode : public ProcedureNode
{
    public:
    IterateData1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target = nullptr,
                               ProcedureNode::NodeContext forEachContext = ProcedureNode::NodeContext::AnalysisContext);
    IterateData1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                               ProcedureNode::NodeContext forEachContext = ProcedureNode::NodeContext::AnalysisContext);
    ~IterateData1DProcedureNode() override = default;

    private:
    // Set up keywords for node
    void setUpKeywords();

    /*
     * Data
     */
    private:
    // Whether to use instantaneous data rather than accumulated data
    bool instantaneous_{true};
    // Collect1D node that we are processing
    std::shared_ptr<const Collect1DProcedureNode> sourceData_;
    std::shared_ptr<const IntegerCollect1DProcedureNode> sourceIntegerData_;

    /*
     * Parameters
     */
    private:
    // Pointers to individual parameters
    std::shared_ptr<ExpressionVariable> xParameter_, valueParameter_;

    /*
     * Branch
     */
    private:
    // Branch for ForEach
    ProcedureNodeSequence forEachBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<ProcedureNodeSequence> branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute Node
    bool execute(const ProcedureContext &procedureContect) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};

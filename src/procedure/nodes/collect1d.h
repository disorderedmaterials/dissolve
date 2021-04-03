// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/histogram1d.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class CalculateProcedureNodeBase;
class SequenceProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Collect1D
class Collect1DProcedureNode : public ProcedureNode
{
    public:
    Collect1DProcedureNode(CalculateProcedureNodeBase *observable = nullptr, double rMin = 0.0, double rMax = 10.0,
                           double binWidth = 0.05);
    ~Collect1DProcedureNode() override = default;

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
    // Observable to bin along x (retrieved from keyword)
    CalculateProcedureNodeBase *xObservable_;
    // Index of x observable data to use (retrieved from keyword)
    int xObservableIndex_;
    // Histogram in which to accumulate data
    OptionalReferenceWrapper<Histogram1D> histogram_;

    public:
    // Return accumulated data
    const Data1D &accumulatedData() const;
    // Return range minimum
    double minimum() const;
    // Return range maximum
    double maximum() const;
    // Return bin width
    double binWidth() const;

    /*
     * Branches
     */
    private:
    // Branch for subcollection (if defined), run if the target quantity is successfully binned
    SequenceProcedureNode *subCollectBranch_;

    public:
    // Add and return subcollection sequence branch
    SequenceProcedureNode *addSubCollectBranch(ProcedureNode::NodeContext context);
    // Return whether this node has a branch
    bool hasBranch() const override;
    // Return SequenceNode for the branch (if it exists)
    SequenceProcedureNode *branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
